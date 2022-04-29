#include "utils.h"

std::list<Theater> theaters;
std::map<std::string, Client> clients;

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

int ClientCall(SOCKET clientSocket, SOCKADDR_IN client)
{
	int ret_val = 0;
	char buf[20]{};
	std::string message, reply;

	const std::string ip_addr(inet_ntop(client.sin_family, &client.sin_addr, buf, 20));
	if (!clients.contains(ip_addr))
	{
		clients.insert(std::make_pair(ip_addr, Client(ip_addr)));
	}

	send(clientSocket, "100 OK", sizeof("100 OK"), 0);
	reply.reserve(2000);
	while (true)
	{
		// Receive location
		int bytesReceived = recv(clientSocket, &reply[0], 2000, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cout << "\nReceive error!\n";
			ret_val = SOCKET_ERROR;
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "\nClient disconnected!\n";
			ret_val = 1;
			break;
		}
		std::cout << reply << '\n';
		// Check if client wants to quit
		if (reply == "QUIT")
		{
			send(clientSocket, "400 BYE", sizeof("400 BYE"), 0);
			std::cout << "Bye, client...\n\n";
			return closesocket(clientSocket);
		}

		// Check for theaters in said location
		auto find_loc = [&](Theater& p) { return p.get_location() == reply; };
		// iterator that points to theater
		auto it = std::find_if(theaters.begin(), theaters.end(), find_loc);

		message = std::to_string(it != theaters.end());
		send(clientSocket, message.data(), message.length() + 1, 0);

		recv(clientSocket, &reply[0], 2000, 0);
		
		// Check shows in said genre
		auto not_rec = [&](Show& show) {
			return show.available_seats > 0
				&& show.genre == reply
				&& !clients[ip_addr].been_recommended(show.id);
		};

		auto no_shows = std::count_if((*it).shows.begin(), (*it).shows.end(), not_rec);
		message = std::to_string(no_shows);
		send(clientSocket, message.data(), message.length() + 1, 0);
		// Send show info in JSON format
		for (auto& show : (*it).shows)
		{
			if (not_rec(show))
			{
				json j(show);
				message = j.dump();
				send(clientSocket, message.data(), message.length() + 1, 0);
				clients[ip_addr].showsRec.push_back(show.id);
			}
		}

		// Receive show info
		recv(clientSocket, &reply[0], 2000, 0);
		json j = json::parse(reply);
		auto p = j.get<std::pair<int, int>>();
		auto show_it = std::find_if((*it).shows.begin(), (*it).shows.end(), [&](Show& s) {return s.id == p.first; });
		clients[ip_addr].showsSeen.push_back(p.first);
		(*show_it).available_seats -= p.second;
	}
	// Close the socket
	closesocket(clientSocket);
	return ret_val;
}