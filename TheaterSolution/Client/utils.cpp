#include "utils.h"

// Function to validate an IP address
// Returns: bool
// true if IP address is valid, false if not
bool validateIP(const std::string& ip_addr)
{
	std::regex re("^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}"
		"([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");
	return std::regex_match(ip_addr, re);
}

// Asks client for location and receives confirmation from server
// Returns: int
// 0/1 -> 0 if location is not found, 1 otherwise
// SOCKET_ERROR -> Any error occured during send or recv
int askLocation(SOCKET& serverSocket)
{
	int ws_result;
	std::string message, reply;
	
	std::cout << "Location: ";
	getline(std::cin, message);
	transform(message.begin(), message.end(), message.begin(), toupper);

	ws_result = send(serverSocket, message.data(), message.length() + 1, 0);
	if (ws_result < 0)
	{
		std::cout << "Send failed\n";
		return SOCKET_ERROR;
	}
	std::cout << "Location sent\n\n";	
	reply.reserve(2000);
	ws_result = recv(serverSocket, &reply[0], 2000, 0);
	if (ws_result <= 0)	return SOCKET_ERROR;

	int ret_val = stoi(reply);
	if (ret_val)
		std::cout << "Location found\n";
	else
		std::cout << "Location not found\n";
	return ret_val;
}

// Quit call with server
// Returns: bool
// true if client wishes to quit, otherwise false
bool quitCall(SOCKET& serverSocket)
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
		return true;
	}
	return false;
}

// Asks client for preferred genre
// Returns: int
// >= 0 -> No of shows available in said genre
// SOCKET_ERROR -> Any error occurred during send or recv
int askGenre(SOCKET& serverSocket)
{
	int ws_result;
	std::string genre, reply;

	std::cout << "Genre: ";
	getline(std::cin, genre);
	transform(genre.begin(), genre.end(), genre.begin(), toupper);

	ws_result = send(serverSocket, genre.data(), genre.length() + 1, 0);
	if (ws_result < 0)
	{
		std::cout << "Send failed\n";
		return SOCKET_ERROR;
	}
	std::cout << "Genre sent\n\n";
	reply.reserve(2000);
	ws_result = recv(serverSocket, &reply[0], 2000, 0);
	if (ws_result <= 0)	return SOCKET_ERROR;

	int no_shows = stoi(reply);
	std::cout << "There are " << (no_shows == 0 ? "no" : std::to_string(no_shows)) << " shows available\n";
	return no_shows;
}

int ServerCall(SOCKET& serverSocket)
{
	bool quit = false;
	std::string message;
	char server_reply[2000];
	int ret_val;

	// Receive a reply from the server
	recv(serverSocket, server_reply, 2000, 0);
	std::cout << server_reply << '\n';

	quit = quitCall(serverSocket);
	while (!quit)
	{
		// Ask for location
		while ((ret_val = askLocation(serverSocket)) == 0)
		{
			if (quit = quitCall(serverSocket)) break;
		}
		if (quit || ret_val == SOCKET_ERROR) break;

		int no_shows = askGenre(serverSocket);
		if (no_shows == SOCKET_ERROR) break;

		std::list<Show> shows;
		for (int i = 0; i < no_shows; i++)
		{
			recv(serverSocket, server_reply, 2000, 0);
			json j = server_reply;
			shows.push_back(j.get<Show>());
		}
	}
	// Close the socket
	return closesocket(serverSocket);
}