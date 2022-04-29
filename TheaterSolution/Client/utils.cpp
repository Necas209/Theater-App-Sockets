#include "utils.h"

std::list<Show> shows;

bool validateIP(const std::string& ip_addr)
{
	std::regex re("^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}"
		"([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");
	return std::regex_match(ip_addr, re);
}

int askLocation(SOCKET& serverSocket)
{
	char reply[2000];
	int ws_result;
	std::string location;
	
	std::cout << "Location: ";
	getline(std::cin, location);
	transform(location.begin(), location.end(), location.begin(), toupper);
	if (location == "QUIT") quitCall(serverSocket);

	ws_result = send(serverSocket, location.data(), location.length() + 1, 0);
	if (ws_result < 0) return SOCKET_ERROR;
	std::cout << "Location sent\n\n";	
	
	ws_result = recv(serverSocket, reply, 2000, 0);
	if (ws_result < 0)	return SOCKET_ERROR;
	int ret_val = atoi(reply);
	if (ret_val)
		std::cout << "Location found.\n";
	else
		std::cout << "Location not found.\n";
	return ret_val;
}

int askGenre(SOCKET& serverSocket)
{
	int ws_result;
	char reply[2000];
	std::string genre;

	std::cout << "Genre: ";
	getline(std::cin, genre);
	transform(genre.begin(), genre.end(), genre.begin(), toupper);

	ws_result = send(serverSocket, genre.data(), genre.length() + 1, 0);
	if (ws_result < 0) return SOCKET_ERROR;
	std::cout << "Genre sent.\n\n";
	
	ws_result = recv(serverSocket, reply, 2000, 0);
	if (ws_result < 0)	return SOCKET_ERROR;
	int no_shows = atoi(reply);
	if (no_shows == 0)
		std::cout << "There are no shows available.\n";
	else
		std::cout << "There are " << no_shows << " show(s) available.\n";
	return no_shows;
}

int quitCall(SOCKET& serverSocket)
{
	std::string option;
	char reply[2000];
	std::cout << "Do you wish to quit? (y/n)\n";
	std::cout << "Option: ";
	getline(std::cin, option);
	if (option == "y")
	{
		send(serverSocket, "QUIT", sizeof("QUIT"), 0);
		recv(serverSocket, reply, 2000, 0);
		std::cout << reply << '\n';
		exit(1);
	}
	return 0;
}

std::pair<int, int> pickShow(SOCKET& serverSocket)
{
	std::pair<int, int> show_info(-1, -1);
	int id;
	
	std::cout << "Available shows: \n";
	for (auto& show : shows)
	{
		show.write();
		std::cout << '\n';
	}

	std::cout << "Choose show (id): ";
	std::cin >> id;
	auto it = std::find_if(shows.begin(), shows.end(), 
		[&](Show& s) { return s.id == id; });
	if (it == shows.end())
	{
		std::cout << "No show with id " << id << " is available.\n";
		return show_info;
	}
	show_info.first = (*it).id;

	int no_tickets;
	std::cout << "How many tickets? ";
	std::cin >> no_tickets;
	if (no_tickets <= 0)
	{
		std::cout << "Invalid number of tickets.\n";
		return show_info;
	}
	else if (no_tickets > (*it).available_seats)
	{
		std::cout << "Not enough tickets available.\n";
		return show_info;
	}
	show_info.second = no_tickets;
	return show_info;
}

int ServerCall(SOCKET& serverSocket)
{
	std::string message;
	char server_reply[2000];
	int ret_val, ws_result = 1;

	// Receive a reply from the server
	recv(serverSocket, server_reply, 2000, 0);
	std::cout << server_reply << '\n';

	while (true)
	{
		// Ask for location
		while ((ret_val = askLocation(serverSocket)) == 0);
		if (ret_val < 0) break;

		int no_shows = askGenre(serverSocket);
		if (no_shows < 0) break;

		for (int i = 0; i < no_shows; i++)
		{
			ws_result = recv(serverSocket, server_reply, 2000, 0);
			if (ws_result <= 0) break;
			json j = json::parse(server_reply);
			shows.push_back(j.get<Show>());
		}
		if (ws_result < 0) break;

		auto p = pickShow(serverSocket);
		if (p.first == -1 || p.second == -1)
		{
			std::cout << "Error occurred during show pick.\n";
		}
		else
		{
			json j{ {"id", p.first},
					{"no_tickets", p.second} };
			message = j.dump();
			ws_result = send(serverSocket, message.data(), message.length() + 1, 0);
		}
	}
	// Close the socket
	return closesocket(serverSocket);
}