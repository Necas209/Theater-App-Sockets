#include "utils.h"
#include <WS2tcpip.h>

constexpr auto DS_TEST_PORT = (USHORT)68000;

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET serverSocket;
	SOCKADDR_IN server_addr{};
	int ws_result;
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
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
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
	server_addr.sin_family = AF_INET;
	inet_pton(server_addr.sin_family, ip_addr.data(), &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(DS_TEST_PORT);
	// Connect to remote server
	ws_result = connect(serverSocket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (ws_result < 0)
	{
		std::cout << "Connection error\n";
		return 1;
	}
	std::cout << "Connected\n";
	main_call(serverSocket);
	// Cleanup winsock
	WSACleanup();
	return 0;
}
