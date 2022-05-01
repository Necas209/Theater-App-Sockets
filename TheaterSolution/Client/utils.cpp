#include "utils.h"

std::list<Show> shows;

bool validate_ip(const std::string& ip_addr)
{
	std::regex re("^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}"
		"([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");
	return std::regex_match(ip_addr, re);
}

int pick_location(SOCKET& serverSocket, std::string& location)
{
	// Send request for available locations
	Message msg(CODE::GET_LOCATIONS, "");
	json j = msg;
	auto message = j.dump();
	int ret_val = send(serverSocket, message.data(), (int)message.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Receive available locations
	char reply[2000];
	ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse response
	j = json::parse(reply);
	msg = j.get<Message>();
	j = json::parse(msg.content);
	std::set<std::string> locations;
	j.get_to(locations);
	// Check number of locations received
	auto n = locations.size();
	if (n == 0) {
		std::cout << "No locations available.\n";
		return 0;
	}
	// Pick location
	int option = 0;
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

int pick_genre(SOCKET& serverSocket, std::string& location, std::string& genre)
{
	// Send request for available genres
	Message msg(CODE::GET_GENRES, location);
	json j = msg;
	auto message = j.dump();
	int ret_val = send(serverSocket, message.data(), (int)message.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Receive available genres
	char reply[2000];
	ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse response
	j = json::parse(reply);
	msg = j.get<Message>();
	j = json::parse(msg.content);
	std::set<std::string> genres;
	j.get_to(genres);
	// Check number of genres
	auto n = genres.size();
	if (n == 0) {
		std::cout << "No genres available.\n";
		return 0;
	}
	// Pick genre
	int option = 0;
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

std::pair<int, int> pick_show(SOCKET& serverSocket)
{
	std::pair<int, int> show_info(-1, -1);
	// Check no of shows
	if (shows.size() == 0)
	{
		std::cout << "No available shows.\n";
		return show_info;
	}
	// Pick show from available shows
	std::cout << "Available shows: \n";
	for (auto& show : shows)
	{
		show.write();
		std::cout << '\n';
	}
	int id;
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
	// Ask how many tickets the client wants
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
	// Pick location
	std::string location;
	int ret_val = pick_location(serverSocket, location);
	if (ret_val < 0) return ret_val;
	// Pick genre
	std::string genre;
	ret_val = pick_genre(serverSocket, location, genre);
	if (ret_val < 0) return ret_val;
	// Ask for shows
	Message msg(CODE::GET_SHOWS, json{ {"location", location},
			{"genre", genre} }.dump());
	json j = msg;
	auto s = j.dump();
	ret_val = send(serverSocket, s.data(), (int)s.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse response
	char reply[2000];
	ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	msg = json::parse(reply).get<Message>();
	int no_shows = std::stoi(msg.content);
	// Get shows
	for (int i = 0; i < no_shows; i++)
	{
		ret_val = recv(serverSocket, reply, 2000, 0);
		if (ret_val <= 0) return SOCKET_ERROR;
		Message msg = json::parse(reply).get<Message>();
		j = json::parse(msg.content);
		shows.push_back(j.get<Show>());
	}
	// Pick show
	auto p = pick_show(serverSocket);
	if (p.first == -1 || p.second == -1)
	{
		std::cout << "Error occurred during show pick.\n";
	}
	// Send show/ticket info
	shows.clear(); // Make sure shows is cleared
	msg = Message(CODE::BUY_TICKETS, json{ {"id", p.first},
			{"no_tickets", p.second} }.dump());
	json k = msg;
	s = k.dump();
	return send(serverSocket, s.data(), (int)s.length() + 1, 0);
}

int main_call(SOCKET& serverSocket)
{
	// Receive a reply from the server
	char server_reply[2000];
	int ret_val = recv(serverSocket, server_reply, 2000, 0);
	if (ret_val <= 0)
	{
		closesocket(serverSocket);
		return ret_val;
	}
	std::cout << server_reply << '\n';
	// Main menu
	while (ret_val != SOCKET_ERROR)
	{
		int option;
		std::cout << "Menu:\n";
		std::cout << "\t1 -> Buy tickets\n";
		std::cout << "\t2 -> Quit\n";
		std::cout << "Option: ";
		(std::cin >> option).ignore();
		switch (option)
		{
		case 1:
			ret_val = buyTickets(serverSocket);
			break;
		case 2:
			ret_val = quit_call(serverSocket);
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

int quit_call(SOCKET& serverSocket)
{
	// Confirm quit
	std::string option;
	std::cout << "Do you wish to quit? (y/n)\n";
	std::cout << "Option: ";
	getline(std::cin, option);
	if (option == "y")
	{
		// Send request to end call
		Message msg(CODE::QUIT, "QUIT");
		json j = msg;
		std::string s = j.dump();
		send(serverSocket, s.data(), (int)s.length() + 1, 0);
		// Receive responde to end call
		char reply[2000];
		recv(serverSocket, reply, 2000, 0);
		msg = json::parse(reply).get<Message>();
		std::cout << msg.content << '\n';
		exit(1);
	}
	return 0;
}