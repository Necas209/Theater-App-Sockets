#include "utils.h"

// Function to validate an IP address
bool validateIP(const std::string& ip_addr)
{
	std::regex re("^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}"
		"([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");
	return std::regex_match(ip_addr, re);
}

int ServerCall(SOCKET& s)
{
	char message[2000];
	char server_reply[2000];
	int recv_size, ws_result, i = 1;

	// Receive a reply from the server
	recv_size = recv(s, server_reply, 2000, 0);
	std::cout << server_reply << '\n';

	while (true)
	{
		// Send some data
		printf("Message %i : ", i);
		std::cin.getline(message, sizeof(message));
		ws_result = send(s, message, strlen(message) + 1, 0);
		if (ws_result < 0)
		{
			std::cout << "Send failed\n";
			return 1;
		}
		std::cout << "Data Sent\n\n";

		// Receive a reply from the server
		recv_size = recv(s, server_reply, 2000, 0);
		if (recv_size == SOCKET_ERROR)
		{
			std::cout << "recv failed\n";
		}
		if (strcmp(server_reply, "400 BYE") == 0)
		{
			std::cout << "Bye, server...\n\n";
			break;
		}
		std::cout << "Reply received\n\n";
		printf("Reply %i : %s\n", i++, server_reply);
	}
	// Close the socket
	return closesocket(s);
}