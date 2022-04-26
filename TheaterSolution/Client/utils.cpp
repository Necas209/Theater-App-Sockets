#include "utils.h"

// Function to validate an IP address
bool validateIP(const std::string& ip_addr)
{
	std::regex re("^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}"
		"([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");
	return std::regex_match(ip_addr, re);
}

int ServerCall(SOCKET& serverSocket)
{
	std::string message;
	char server_reply[2000];
	int recv_size, ws_result, i = 0;

	// Receive a reply from the server
	recv_size = recv(serverSocket, server_reply, 2000, 0);
	std::cout << server_reply << '\n';

	// Ask for location
	std::cout << "Localização: ";
	std::getline(std::cin, message);
	ws_result = send(serverSocket, message.data(), message.length() + 1, 0);
	if (ws_result < 0)
	{
		std::cout << "Send failed\n";
		return 1;
	}
	std::cout << "Data Sent\n\n";

	// Receive a reply from the server
	recv_size = recv(serverSocket, server_reply, 2000, 0);
	if (recv_size == SOCKET_ERROR)
	{
		std::cout << "recv failed\n";
	}
	if (strcmp(server_reply, "400 BYE") == 0)
	{
		std::cout << "Bye, server...\n\n";
	}

	std::cout << "Reply received\n\n";
	printf("Reply %i : %s\n", i++, server_reply);

	// Close the socket
	return closesocket(serverSocket);
}