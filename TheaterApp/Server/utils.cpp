#include "utils.h"

std::list<Theater> theaters;
std::map<std::string, Client> clients;
std::mutex tickets_mutex;
std::mutex log_mutex;
thread_local std::string ip_addr;

void log_message(Message msg, SENDER s)
{
	// Lock access to logs
	std::lock_guard<std::mutex> guard(log_mutex);
	// Log message
	Log log(msg, s, ip_addr);
	const std::string path = log_path(log);
	// Check if file exists
	// If so, append rather than write
	if (std::filesystem::exists(path))
	{
		std::ofstream ofs{ path, std::ios_base::app };
		ofs << log;
	}
	else
	{
		std::ofstream ofs{ path };
		ofs << log;
	}
}

void read_theaters(const char* filename)
{
	std::ifstream ifs{ filename };
	json j;
	ifs >> j;
	j.get_to(theaters);
}

void write_theaters(const char* filename)
{
	std::ofstream ofs{ filename };
	json j = theaters;
	ofs << std::setw(4) << j;
}

void read_clients(const char* filename)
{
	std::ifstream ifs{ filename };
	json j;
	ifs >> j;
	j.get_to(clients);
}

void write_clients(const char* filename)
{
	std::ofstream ofs{ filename };
	json j = clients;
	ofs << std::setw(4) << j;
}

int get_locations(SOCKET& clientSocket)
{
	// Make set of (non-repeated) locations
	std::set<std::string> locations;
	for (auto& theater : theaters)
	{
		locations.insert(theater.location);
	}
	// Send locations to client
	json j = locations;
	Message msg(CODE::GET_LOCATIONS, j.dump());
	json k = msg;
	auto s = k.dump();
	int ret_val = send(clientSocket, s.data(), (int)s.length() + 1, 0);
	log_message(msg, SENDER::SERVER); // log message
	return ret_val;
}

int get_genres(SOCKET& clientSocket, Message& msg)
{
	// Find theater in given location
	auto& location = msg.content;
	auto it = std::find_if(theaters.begin(), theaters.end(), [&](Theater& t)
		{ return t.location == location; });
	if (it == theaters.end())
		return -1;
	// Make set of (non-repeated) genres
	std::set<std::string> genres;
	for (auto& show : (*it).shows)
	{
		genres.insert(show.genre);
	}
	// Send genres to client
	json j = genres;
	msg = Message(CODE::GET_GENRES, j.dump());
	json k = msg;
	auto s = k.dump();
	int ret_val = send(clientSocket, s.data(), (int)s.length() + 1, 0);
	log_message(msg, SENDER::SERVER); // log message
	return ret_val;
}

int get_shows(SOCKET& clientSocket, Message& msg, theater_it& it)
{
	// Parse message content for location and genre
	json j = json::parse(msg.content);
	std::string location, genre;
	j.at("location").get_to(location);
	j.at("genre").get_to(genre);
	// Find theater in given location
	it = std::find_if(theaters.begin(), theaters.end(),
		[&](Theater& t) { return t.location == location; });
	if (it != theaters.end())
	{
		// Filter shows
		std::list<Show> shows;
		std::copy_if((*it).shows.begin(), (*it).shows.end(), std::back_inserter(shows),
			[&](Show& s) {
				return s.available_seats > 0
					&& s.genre == genre
					&& !clients[ip_addr].been_recommended(s.id);
			});
		// Add to recommended
		std::for_each(shows.begin(), shows.end(), 
			[&](Show& s) { clients[ip_addr].showsRec.push_back(s.id); });
		// Send shows to client
		json j = shows;
		Message msg(CODE::GET_SHOWS, j.dump());
		json k = msg;
		auto s = k.dump();
		int ret_val = send(clientSocket, s.data(), (int)s.length() + 1, 0);
		log_message(msg, SENDER::SERVER); // log message
		if (ret_val <= 0) return SOCKET_ERROR;
	}
	return 0;
}

int buy_tickets(SOCKET& clientSocket, Message& msg)
{
	// Lock access to shows
	std::lock_guard<std::mutex> guard(tickets_mutex);
	// Send available shows to client
	theater_it it;
	int ret_val = get_shows(clientSocket, msg, it);
	if (ret_val < 0) return SOCKET_ERROR;
	// Get ticket info from client
	char reply[2000];
	ret_val = recv(clientSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse ticket info
	msg = json::parse(reply).get<Message>();
	log_message(msg, SENDER::CLIENT); // log message
	json j = json::parse(msg.content);
	int id, no_tickets;
	j.at("id").get_to(id);
	j.at("no_tickets").get_to(no_tickets);
	if (id != -1 && no_tickets != -1)
	{
		// Update client's seen shows
		clients[ip_addr].showsSeen.push_back(id);
		// Update available seats
		auto show_it = std::find_if((*it).shows.begin(), (*it).shows.end(),
			[&](Show& s) { return s.id == id; });
		(*show_it).available_seats -= no_tickets;
	}
	return ret_val;
}

int main_call(SOCKET clientSocket, SOCKADDR_IN client_addr)
{
	// Get client's IP address
	char buf[20];
	inet_ntop(client_addr.sin_family, &client_addr.sin_addr, buf, 20);
	ip_addr = std::string(buf);
	if (!clients.contains(ip_addr))
	{
		clients.insert(std::make_pair(ip_addr, Client(ip_addr)));
	}
	// Send HELLO message
	Message msg(CODE::HELLO, "100 OK");
	json j = msg;
	auto s = j.dump();
	int ret_val = send(clientSocket, s.data(), (int)s.length() + 1, 0);
	log_message(msg, SENDER::SERVER); // log message
	std::cout << "Hello, client!\n\n";
	while (ret_val > 0)
	{
		// Receive next message and parse it
		char reply[2000];
		ret_val = recv(clientSocket, reply, 2000, 0);
		if (ret_val <= 0) break;
		Message message = json::parse(reply).get<Message>();
		log_message(message, SENDER::CLIENT); // log message
		// Call proper function, according to message code
		switch (message.code)
		{
		case CODE::GET_LOCATIONS:
			ret_val = get_locations(clientSocket);
			break;
		case CODE::GET_GENRES:
			ret_val = get_genres(clientSocket, message);
			break;
		case CODE::GET_SHOWS:
			ret_val = buy_tickets(clientSocket, message);
			break;
		case CODE::QUIT:
			ret_val = quit_call(clientSocket);
			break;
		default:
			std::cout << "Invalid message code.\n";
			break;
		}
	}
	// Close the socket
	closesocket(clientSocket);
	return ret_val;
}

int quit_call(SOCKET& clientSocket)
{
	// Send 400 BYE message to client
	Message msg(CODE::QUIT, "400 BYE");
	json j = msg;
	auto s = j.dump();
	send(clientSocket, s.data(), (int)s.length() + 1, 0);
	log_message(msg, SENDER::SERVER); // log message
	std::cout << "Bye, client...\n\n";
	return 0;
}