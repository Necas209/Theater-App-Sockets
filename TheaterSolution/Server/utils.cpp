#include "utils.h"

std::list<Theater> theaters;
std::map<std::string, Client> clients;
std::mutex tickets_mutex;

void WriteTheaters()
{
	for (auto& theater : theaters)
	{
		theater.write();
	}
}

void ReadTheatersFromFile(const char* filename)
{
	std::ifstream ifs{ filename };
	json j;
	ifs >> j;
	j.get_to(theaters);
}

void WriteTheatersToFile(const char* filename)
{
	std::ofstream ofs{ filename };
	json j(theaters);
	ofs << j;
}

void ReadClientsFromFile(const char* filename)
{
	std::ifstream ifs{ filename };
	json j;
	ifs >> j;
	j.get_to(clients);
}

void WriteClientsToFile(const char* filename)
{
	std::ofstream ofs{ filename };
	json j(clients);
	ofs << j;
}

int sendLocations(SOCKET& clientSocket)
{
	std::set<std::string> locations;
	for (auto& theater : theaters)
	{
		locations.insert(theater.location);
	}
	json j = locations;
	Message msg(CODE::GET_LOCATIONS, j.dump());
	json k = msg;
	std::string s = k.dump();
	return send(clientSocket, s.data(), s.length() + 1, 0);
}

int sendGenres(SOCKET& clientSocket, Message& msg)
{
	std::string location = msg.content;
	auto it = std::find_if(theaters.begin(), theaters.end(), [&](Theater& t) {return t.location == location; });
	if (it == theaters.end())
		return -1;

	std::set<std::string> genres;
	for (auto& show : (*it).shows)
	{
		genres.insert(show.genre);
	}
	json j = genres;
	msg = Message(CODE::GET_GENRES, j.dump());
	json k = msg;
	std::string s = k.dump();
	return send(clientSocket, s.data(), s.length() + 1, 0);
}

int sendShows(SOCKET& clientSocket, Message& msg, const std::string& ip_addr, std::list<Theater>::iterator& it)
{
	json j = json::parse(msg.content);
	std::string location, genre;
	j.at("location").get_to(location);
	j.at("genre").get_to(genre);
	// Find theater in given location
	it = std::find_if(theaters.begin(), theaters.end(), [&](Theater& t) {return t.location == location; });
	if (it != theaters.end())
	{
		// Check shows in given genre
		auto not_rec = [&](Show& show) {
			return show.available_seats > 0
				&& show.genre == genre
				&& !clients[ip_addr].been_recommended(show.id);
		};
		// Count number of shows
		auto no_shows = std::count_if((*it).shows.begin(), (*it).shows.end(), not_rec);
		// Send information to client
		json j = Message(CODE::GET_SHOWS, std::to_string(no_shows));
		std::string s = j.dump();
		int ret_val = send(clientSocket, s.data(), s.length() + 1, 0);
		if (ret_val <= 0) return SOCKET_ERROR;
		// Send show info in JSON format
		for (auto& show : (*it).shows)
		{
			if (not_rec(show))
			{
				json j = show;
				json k = Message(CODE::GET_SHOWS, j.dump());
				s = k.dump();
				int ret_val = send(clientSocket, s.data(), s.length() + 1, 0);
				if (ret_val <= 0) return SOCKET_ERROR;
				clients[ip_addr].showsRec.push_back(show.id);
			}
		}
	}
	return 0;
}

int buyTickets(SOCKET& clientSocket, Message& msg, const std::string& ip_addr)
{
	std::list<Theater>::iterator it;
	// Send available shows to client
	std::lock_guard<std::mutex> guard(tickets_mutex);
	int ret_val = sendShows(clientSocket, msg, ip_addr, it);
	if (ret_val < 0) return SOCKET_ERROR;
	// Get ticket info from client
	char reply[2000];
	ret_val = recv(clientSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse ticket info
	Message m = json::parse(reply).get<Message>();
	json j = json::parse(m.content);
	int id = j.at("id").get<int>();
	int no_tickets = j.at("no_tickets").get<int>();
	// Update available seats and client's seen shows
	if (id != -1 && no_tickets != -1)
	{
		auto show_it = std::find_if((*it).shows.begin(), (*it).shows.end(), 
			[&](Show& s) {return s.id == id; });
		clients[ip_addr].showsSeen.push_back(id);
		(*show_it).available_seats -= no_tickets;
	}
	return ret_val;
}

int ClientCall(SOCKET clientSocket, SOCKADDR_IN client)
{
	int ret_val = 1;
	char buf[20]{}, reply[2000];
	std::string message;

	const std::string ip_addr(inet_ntop(client.sin_family, &client.sin_addr, buf, 20));
	if (!clients.contains(ip_addr))
	{
		clients.insert(std::make_pair(ip_addr, Client(ip_addr)));
	}

	ret_val = send(clientSocket, "100 OK", sizeof("100 OK"), 0);
	while (ret_val > 0)
	{
		int ret_val = recv(clientSocket, reply, 2000, 0);
		if (ret_val <= 0) break;
		Message msg = json::parse(reply).get<Message>();

		switch (msg.code)
		{
		case CODE::GET_LOCATIONS:
			ret_val = sendLocations(clientSocket);
			break;
		case CODE::GET_GENRES:
			ret_val = sendGenres(clientSocket, msg);
			break;
		case CODE::GET_SHOWS:
			ret_val = buyTickets(clientSocket, msg, ip_addr);
			break;
		case CODE::QUIT:
			ret_val = quitCall(clientSocket);
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

int quitCall(SOCKET& clientSocket)
{
	Message msg(CODE::QUIT, "400 BYE");
	json j = msg;
	std::string s = j.dump();
	int ret_val = send(clientSocket, s.data(), s.length() + 1, 0);
	std::cout << "Bye, client...\n\n";
	return ret_val;
}