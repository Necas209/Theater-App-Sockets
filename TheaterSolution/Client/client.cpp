/*
	Simple winsock client
*/

#include "utils.h"

#pragma warning(disable : 4996)

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	SOCKADDR_IN server{};
	int ws_result;

	// Initialise winsock
	std::cout << "\nInitialising Winsock...";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	// Create a socket
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	std::cout << "Socket created.\n";

	// Ask client for server ip address
	std::string ip_addr;
	std::cout << "Server IP address: ";
	getline(std::cin, ip_addr);
	if (!validateIP(ip_addr))
	{
		ip_addr = "25.63.23.179";
		std::cout << "Invalid IP address.\nIP will default to " << ip_addr << ".\n";
	}
	// create the socket  address (ip address and port)
	server.sin_addr.s_addr = inet_addr(ip_addr.data());
	server.sin_family = AF_INET;
	server.sin_port = htons((u_short)68000);

	// Connect to remote server
	ws_result = connect(s, (SOCKADDR*)&server, sizeof(server));
	if (ws_result < 0)
	{
		std::cout << "Connection error\n";
		return 1;
	}

	std::cout << "Connected\n";

	ServerCall(s);
	// Cleanup winsock
	WSACleanup();

	return 0;
}
