#include "utils.h"
#include <WS2tcpip.h>

constexpr auto ds_test_port = static_cast<u_short>(68000);

int main()
{
	WSADATA wsa;
	SetConsoleCP(CP_UTF8);
	// Initialise winsock
	std::cout << "\nInitialising Winsock...";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cerr << "Failed. Error Code : " << WSAGetLastError() << '\n';
		return 1;
	}
	std::cout << "Initialised.\n";
	// Create a socket
	const SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == INVALID_SOCKET)
	{
		std::cerr << "Could not create socket : " << WSAGetLastError() << '\n';
		return 1;
	}
	std::cout << "Socket created.\n";
	// Ask client for server IP address
	std::string ip_addr;
	std::cout << "Server IP address: ";
	getline(std::cin, ip_addr);
	if (!validate_ip(ip_addr))
	{
		ip_addr = "172.30.217.49";
		std::cout << "Invalid IP address.\n";
		std::cout << "IP address will default to " << ip_addr << ".\n";
	}
	// create the socket address (IP address and port)
	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ds_test_port);
	inet_pton(server_addr.sin_family, ip_addr.data(), &server_addr.sin_addr.s_addr);
	// Connect to remote server
	if (connect(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
	{
		std::cout << "Connection error\n";
		return 1;
	}
	std::cout << "Connected\n";
	main_call(server_socket);
	// Cleanup winsock
	WSACleanup();
	return 0;
}
