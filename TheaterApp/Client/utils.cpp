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
	json j = Message(CODE::GET_LOCATIONS, "");
	auto message = j.dump();
	int ret_val = send(serverSocket, message.data(), (int)message.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Receive available locations
	char reply[2000];
	ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse response
	Message msg = json::parse(reply).get<Message>();
	std::set<std::string> locations;
	json::parse(msg.content).get_to(locations);
	// Pick location
	auto n = locations.size();
	int option = 0;
	do {
		std::cout << "Available locations:\n";
		int i = 0;
		std::for_each(locations.begin(), locations.end(),
			[&](const auto& l) { std::cout << '\t' << i << " -> " << l << '\n'; i++; });
		std::cout << "Option: ";
		(std::cin >> option).ignore();
	} while (option < 0 || option >= n);
	location = *std::next(locations.begin(), option);
	return 0;
}

int pick_genre(SOCKET& serverSocket, std::string& location, std::string& genre)
{
	// Send request for available genres
	json j = Message(CODE::GET_GENRES, location);
	auto message = j.dump();
	int ret_val = send(serverSocket, message.data(), (int)message.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Receive available genres
	char reply[2000];
	ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse response
	Message msg = json::parse(reply).get<Message>();
	std::set<std::string> genres;
	json::parse(msg.content).get_to(genres);
	// Pick genre
	auto n = genres.size();
	int option = 0;
	do {
		std::cout << "Available genres:\n";
		int i = 0;
		std::for_each(genres.begin(), genres.end(),
			[&](const auto& g) { std::cout << '\t' << i << " -> " << g << '\n'; i++; });
		std::cout << "Option: ";
		(std::cin >> option).ignore();
	} while (option < 0 || option >= n);
	genre = *std::next(genres.begin(), option);
	return 0;
}

auto& pick_show(SOCKET& serverSocket)
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
	std::for_each(shows.begin(), shows.end(), [](Show& s) { s.write(); std::cout << '\n'; });
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

int buy_tickets(SOCKET& serverSocket)
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
	json j = Message(CODE::GET_SHOWS, json{ {"location", location},
			{"genre", genre} }.dump());
	auto s = j.dump();
	ret_val = send(serverSocket, s.data(), (int)s.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse response and get shows
	char reply[2000];
	ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	Message msg = json::parse(reply).get<Message>();
	json::parse(msg.content).get_to(shows);
	// Pick show
	auto& p = pick_show(serverSocket);
	if (p.first == -1 || p.second == -1)
	{
		std::cout << "Error occurred during show pick.\n";
	}
	// Send show/ticket info
	shows.clear(); // Make sure shows is cleared
	json k = Message(CODE::BUY_TICKETS, json{ {"id", p.first},
			{"no_tickets", p.second} }.dump());
	s = k.dump();
	ret_val = send(serverSocket, s.data(), (int)s.length() + 1, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	return 0;
}

int main_call(SOCKET& serverSocket)
{
	// Receive HELLO message
	char reply[2000];
	int ret_val = recv(serverSocket, reply, 2000, 0);
	if (ret_val <= 0)
	{
		closesocket(serverSocket);
		return ret_val;
	}
	Message msg = json::parse(reply).get<Message>();
	std::cout << msg.content << '\n\n';
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
			ret_val = buy_tickets(serverSocket);
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
		json j = Message(CODE::QUIT, "QUIT");
		std::string s = j.dump();
		int ret_val = send(serverSocket, s.data(), (int)s.length() + 1, 0);
		if (ret_val <= 0) return SOCKET_ERROR;
		// Receive response to end call
		char reply[2000];
		ret_val = recv(serverSocket, reply, 2000, 0);
		if (ret_val <= 0) return SOCKET_ERROR;
		Message msg = json::parse(reply).get<Message>();
		std::cout << msg.content << '\n';
		return -1;
	}
	return 0;
}