#include "utils.h"

std::list<Theater> theaters;
std::map<std::string, Client> clients;
std::mutex tickets_mutex;

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
	return send(clientSocket, s.data(), (int)s.length() + 1, 0);
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
	return send(clientSocket, s.data(), (int)s.length() + 1, 0);
}

int get_shows(SOCKET& clientSocket, Message& msg, const std::string& ip_addr, theater_it& it)
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
		// Count no of shows not yet recommended to client
		auto not_rec = [&](Show& show) {
			return show.available_seats > 0
				&& show.genre == genre
				&& !clients[ip_addr].been_recommended(show.id);
		};
		auto no_shows = std::count_if((*it).shows.begin(), (*it).shows.end(), not_rec);
		// Send no of shows to client
		json j = Message(CODE::GET_SHOWS, std::to_string(no_shows));
		auto s = j.dump();
		int ret_val = send(clientSocket, s.data(), (int)s.length() + 1, 0);
		if (ret_val <= 0) return SOCKET_ERROR;
		// Send show info in JSON format
		for (auto& show : (*it).shows)
		{
			if (not_rec(show))
			{
				json j = show;
				json k = Message(CODE::GET_SHOWS, j.dump());
				s = k.dump();
				int ret_val = send(clientSocket, s.data(), (int)s.length() + 1, 0);
				if (ret_val <= 0) return SOCKET_ERROR;
				// Add show to client's recommended
				clients[ip_addr].showsRec.push_back(show.id);
			}
		}
	}
	return 0;
}

int buy_tickets(SOCKET& clientSocket, Message& msg, const std::string& ip_addr)
{
	// Send available shows to client
	std::lock_guard<std::mutex> guard(tickets_mutex);
	theater_it it;
	int ret_val = get_shows(clientSocket, msg, ip_addr, it);
	if (ret_val < 0) return SOCKET_ERROR;
	// Get ticket info from client
	char reply[2000];
	ret_val = recv(clientSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse ticket info
	Message m = json::parse(reply).get<Message>();
	json j = json::parse(m.content);
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
	const std::string ip_addr(buf);
	if (!clients.contains(ip_addr))
	{
		clients.insert(std::make_pair(ip_addr, Client(ip_addr)));
	}
	// Properly start call
	int ret_val = send(clientSocket, "100 OK", sizeof("100 OK"), 0);
	char reply[2000];
	while (ret_val > 0)
	{
		// Receive next message and parse it
		ret_val = recv(clientSocket, reply, 2000, 0);
		if (ret_val <= 0) break;
		Message msg = json::parse(reply).get<Message>();
		// Call proper function, according to message code
		switch (msg.code)
		{
		case CODE::GET_LOCATIONS:
			ret_val = get_locations(clientSocket);
			break;
		case CODE::GET_GENRES:
			ret_val = get_genres(clientSocket, msg);
			break;
		case CODE::GET_SHOWS:
			ret_val = buy_tickets(clientSocket, msg, ip_addr);
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
	std::cout << "Bye, client...\n\n";
	return 0;
}