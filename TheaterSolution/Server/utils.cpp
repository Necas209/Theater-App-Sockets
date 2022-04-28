#include "utils.h"

std::map<std::string, Theater> theaters;
std::map<std::string, Client> clients;

void WriteTheaters()
{
	for (auto& [name, theater] : theaters)
	{
		theater.write();
	}
}

void ReadTheatersFromFile(const char* filename)
{
	io::CSVReader<7> reader(filename);
	reader.read_header(io::ignore_no_column,
		"theater", "location", "id", "name", "genre", "datetime", "capacity", "available_seats");

	std::string theater_name, location, name, genre, datetime;
	int id, capacity, available_seats;

	while (reader.read_row(theater_name, location, id, name,
		genre, datetime, capacity, available_seats))
	{
		if (!theaters.contains(theater_name))
		{
			theaters.insert(std::make_pair(theater_name, Theater(name, location)));
		}
		std::istringstream ss{ datetime };
		tm dt;
		ss >> std::get_time(&dt, "%a %b %d %H:%M:%S %Y");
		theaters[theater_name].shows.push_back(Show(id, name, genre, dt, capacity, available_seats));
	}
}

void WriteTheatersToFile(const char* filename)
{
	std::ofstream ofs{ filename };
	ofs << "theater,location,id,name,genre,datetime,capacity,available_seats\n";
	for (auto& [name, theater] : theaters)
	{
		theater.write_file(ofs);
	}
}

void ReadClientsFromFile(const char* filename)
{
	std::ifstream ifs{ filename };
	json j;
	ifs >> j;
	for (auto& [key, value] : j.items())
	{
		auto client = value.get<Client>();
		clients.insert(std::make_pair(key, client));
	}
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
	const std::string ip_addr(inet_ntoa(client.sin_addr));
	if (!clients.contains(ip_addr))
	{
		clients.insert(std::make_pair(ip_addr, Client(ip_addr)));
	}
	std::string message = "100 OK", reply;
	reply.reserve(2000);
	send(clientSocket, message.data(), message.length() + 1, 0);

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
		using pair = std::pair<std::string, Theater>;
		auto find_loc = [&](pair& p) { return p.second.get_location() == reply; };
		auto it = std::find_if(theaters.begin(), theaters.end(), find_loc);

		message = std::to_string(it != theaters.end());
		send(clientSocket, message.data(), message.length() + 1, 0);

		// Check shows in said genre
		auto& theater = (*it).second;
		auto not_rec = [&](Show& show) {
			return !clients[ip_addr].been_recommended(show.id)
				&& show.available_seats > 0;
		};
		int no_shows = std::count_if(theater.shows.begin(), theater.shows.end(), not_rec);
		message = std::to_string(no_shows);
		send(clientSocket, message.data(), message.length() + 1, 0);
		// Send show info in JSON format
		for (auto& show : theater.shows)
		{
			if (not_rec(show))
			{
				json j(show);
				message = j.dump();
				send(clientSocket, message.data(), message.length() + 1, 0);
			}
		}
	}
	// Close the socket
	closesocket(clientSocket);
	return ret_val;
}