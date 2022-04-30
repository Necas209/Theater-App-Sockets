#include "utils.h"

std::list<Show> shows;

bool validateIP(const std::string& ip_addr)
{
	std::regex re("^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}"
		"([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");
	return std::regex_match(ip_addr, re);
}

int askLocation(SOCKET& serverSocket, std::string& location)
{
	int ret_val, option = -1;
	std::string message;
	Message msg(CODE::GET_LOCATIONS, "");
	json j = msg;
	message = j.dump();
	ret_val = send(serverSocket, message.data(), message.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	char reply[2000];
	ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	j = json::parse(reply);
	msg = j.get<Message>();
	j = json::parse(msg.content);
	auto locations = j.get<std::set<std::string>>();
	int n = locations.size();
	if (n == 0) {
		std::cout << "No locations available.\n";
		return 0;
	}
	do {
		int i = 0;
		std::cout << "Available locations:\n";
		for (const auto& location : locations)
		{
			std::cout << '\t' << i << " -> " << location << '\n';
			++i;
		}
		std::cout << "Option: ";
		(std::cin >> option).ignore();
	} while (option < 0 || option >= n);
	location = *std::next(locations.begin(), option);
	return 0;
}

int askGenre(SOCKET& serverSocket, std::string& location, std::string& genre)
{
	int ret_val, option = -1;
	std::string message;
	Message msg(CODE::GET_GENRES, location);
	json j = msg;
	message = j.dump();
	ret_val = send(serverSocket, message.data(), message.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	char reply[2000];
	ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	j = json::parse(reply);
	msg = j.get<Message>();
	j = json::parse(msg.content);
	auto genres = j.get<std::set<std::string>>();
	int n = genres.size();
	if (n == 0) {
		std::cout << "No genres available.\n";
		return 0;
	}
	do {
		int i = 0;
		std::cout << "Available genres:\n";
		for (const auto& genre : genres)
		{
			std::cout << '\t' << i << " -> " << genre << '\n';
			++i;
		}
		std::cout << "Option: ";
		(std::cin >> option).ignore();
	} while (option < 0 || option >= n);
	genre = *std::next(genres.begin(), option);
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
	(std::cin >> id).ignore();
	auto it = std::find_if(shows.begin(), shows.end(), 
		[&](Show& s) { return s.id == id; });
	if (it == shows.end())
	{
		std::cout << "No show with id " << id << " is available.\n";
		return show_info;
	}
	show_info.first = id;

	int no_tickets;
	std::cout << "How many tickets? ";
	(std::cin >> no_tickets).ignore();
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

int buyTickets(SOCKET& serverSocket)
{
	int ret_val = 0;
	std::string location, genre;
	// Ask for location
	ret_val = askLocation(serverSocket, location);
	if (ret_val < 0) return ret_val;
	// Ask for genre
	ret_val = askGenre(serverSocket, location, genre);
	if (ret_val < 0) return ret_val;
	// Ask for shows
	json j = json{ {"location", location},
			{"genre", genre} };
	Message msg(CODE::GET_SHOWS, j.dump());
	json k = msg;
	std::string s = k.dump();
	ret_val = send(serverSocket, s.data(), s.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Get shows
	char reply[2000];
	ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	msg = json::parse(reply).get<Message>();
	int no_shows = std::stoi(msg.content);
	for (int i = 0; i < no_shows; i++)
	{
		ret_val = recv(serverSocket, reply, 2000, 0);
		if (ret_val <= 0) return SOCKET_ERROR;
		Message msg = json::parse(reply).get<Message>();
		json j = json::parse(msg.content);
		shows.push_back(j.get<Show>());
	}
	// Pick show
	if (shows.size() == 0)
	{
		std::cout << "No available shows.\n";
		return 0;
	}
	auto p = pickShow(serverSocket);
	if (p.first == -1 || p.second == -1)
	{
		std::cout << "Error occurred during show pick.\n";
	}
	// Send ticket info
	msg = Message(CODE::BUY_TICKETS, json{ {"id", p.first},
			{"no_tickets", p.second} }.dump());
	json l = msg;
	s = l.dump();
	ret_val = send(serverSocket, s.data(), s.length() + 1, 0);
	shows.clear();
	return ret_val;
}

int ServerCall(SOCKET& serverSocket)
{
	std::string message;
	char server_reply[2000];
	int ret_val;

	// Receive a reply from the server
	ret_val = recv(serverSocket, server_reply, 2000, 0);
	if (ret_val <= 0)
	{
		closesocket(serverSocket);
		return ret_val;
	}
	std::cout << server_reply << '\n';

	while (ret_val != SOCKET_ERROR)
	{
		int option;
		std::cout << "Menu:\n";
		std::cout << "\t1 -> Buy tickets\n";
		std::cout << "\t2 -> Quit\n";
		(std::cin >> option).ignore();
		switch (option)
		{
		case 1:
			ret_val = buyTickets(serverSocket);
			break;
		case 2:
			ret_val = quitCall(serverSocket);
			break;
		default:
			std::cout << "Invalid option. Please try again.\n";
			break;
		}
	}
	// Close the socket
	closesocket(serverSocket);
	return ret_val;
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
		Message msg(CODE::QUIT, "QUIT");
		json j = msg;
		std::string s = j.dump();
		send(serverSocket, s.data(), s.length() + 1, 0);
		recv(serverSocket, reply, 2000, 0);
		msg = json::parse(reply).get<Message>();
		std::cout << msg.content << '\n';
		exit(1);
	}
	return 0;
}