#include "utils.h"

std::list<Theater> theaters;
std::map<std::string, Client> clients;
std::mutex tickets_mutex;
std::mutex log_mutex;
thread_local std::string ip_addr;

void log_message(const Message& msg, const SENDER sender)
{
	// Lock access to logs
	std::lock_guard guard(log_mutex);
	// Log message
	const Log log(msg, sender, ip_addr);
	const std::string path = log_path(log);
	std::ofstream ofs{ path, std::ios_base::app };
	ofs << log;
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
	const json j = theaters;
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
	const json j = clients;
	ofs << std::setw(4) << j;
}

int get_locations(const SOCKET& client_socket)
{
	// Make set of (non-repeated) locations
	std::set<std::string> locations;
	for (auto& theater : theaters)
	{
		locations.insert(theater.location);
	}
	// Send locations to client
	const json j = locations;
	Message msg(CODE::GET_LOCATIONS, j.dump());
	const json k = msg;
	const auto s = k.dump();
	const int ret_val = send(client_socket, s.data(), s.length() + 1, 0);
	log_message(msg, SENDER::SERVER); // log message
	return ret_val;
}

int get_genres(const SOCKET& client_socket, Message& msg)
{
	// Find theater in given location
	const auto& location = msg.content;
	const auto it = std::ranges::find_if(theaters, [&](const Theater& t)
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
	const json j = genres;
	msg = Message(CODE::GET_GENRES, j.dump());
	const json k = msg;
	const auto s = k.dump();
	const int ret_val = send(client_socket, s.data(), s.length() + 1, 0);
	log_message(msg, SENDER::SERVER); // log message
	return ret_val;
}

int get_shows(const SOCKET& client_socket, Message& msg, std::list<Theater>::iterator& it)
{
	// Parse message content for location and genre
	json j = json::parse(msg.content);
	std::string location, genre;
	j.at("location").get_to(location);
	j.at("genre").get_to(genre);
	// Find theater in given location
	it = std::ranges::find_if(theaters, [&](const Theater& t) { return t.location == location; });
	if (it != theaters.end())
	{
		// Filter shows
		std::list<Show> shows;
		std::ranges::copy_if((*it).shows, std::back_inserter(shows),
			[&](const Show& s) {
				return s.available_seats > 0
					&& s.genre == genre
					&& !clients[ip_addr].been_recommended(s.id);
			});
		// Add to recommended
		std::ranges::for_each(shows, [&](const Show& s) { clients[ip_addr].shows_rec.push_back(s.id); });
		// Send shows to client
		const json js = shows;
		Message message(CODE::GET_SHOWS, js.dump());
		const json k = message;
		const auto s = k.dump();
		const int ret_val = send(client_socket, s.data(), s.length() + 1, 0);
		log_message(message, SENDER::SERVER); // log message
		if (ret_val <= 0) return SOCKET_ERROR;
	}
	return 0;
}

int buy_tickets(const SOCKET& client_socket, Message& msg)
{
	// Lock access to shows
	std::lock_guard guard(tickets_mutex);
	// Send available shows to client
	std::list<Theater>::iterator it;
	int ret_val = get_shows(client_socket, msg, it);
	if (ret_val < 0) return SOCKET_ERROR;
	// Get ticket info from client
	char reply[2000];
	ret_val = recv(client_socket, reply, 2000, 0);
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
		clients[ip_addr].shows_seen.push_back(id);
		// Update available seats
		const auto show_it = std::ranges::find_if((*it).shows,
			[&](const Show& s) { return s.id == id; });
		(*show_it).available_seats -= no_tickets;
	}
	return ret_val;
}

int main_call(const SOCKET client_socket, const SOCKADDR_IN client_addr)
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
	const json j = msg;
	const auto s = j.dump();
	int ret_val = send(client_socket, s.data(), s.length() + 1, 0);
	log_message(msg, SENDER::SERVER); // log message
	std::cout << "Hello, client!\n\n";
	while (ret_val > 0)
	{
		// Receive next message and parse it
		char reply[2000];
		ret_val = recv(client_socket, reply, 2000, 0);
		if (ret_val <= 0) break;
		auto message = json::parse(reply).get<Message>();
		log_message(message, SENDER::CLIENT); // log message
		// Call proper function, according to message code
		switch (message.code)
		{
		case CODE::GET_LOCATIONS:
			ret_val = get_locations(client_socket);
			break;
		case CODE::GET_GENRES:
			ret_val = get_genres(client_socket, message);
			break;
		case CODE::GET_SHOWS:
			ret_val = buy_tickets(client_socket, message);
			break;
		case CODE::QUIT:
			ret_val = quit_call(client_socket);
			break;
		default:
			std::cout << "Invalid message code.\n";
			break;
		}
	}
	// Close the socket
	closesocket(client_socket);
	return ret_val;
}

int quit_call(const SOCKET& client_socket)
{
	// Send 400 BYE message to client
	Message msg(CODE::QUIT, "400 BYE");
	const json j = msg;
	const auto s = j.dump();
	send(client_socket, s.data(), s.length() + 1, 0);
	log_message(msg, SENDER::SERVER); // log message
	std::cout << "Bye, client...\n\n";
	return 0;
}