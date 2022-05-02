#include "utils.h"
#include <thread>

constexpr auto DS_TEST_PORT = (USHORT)68000;
constexpr auto MAX_THREADS = 5;

int main(int argc, char* argv[])
{
	//
	SetConsoleOutputCP(CP_UTF8);
	// Initialise winsock
	WSADATA wsData;
	std::cout << "Initialising Winsock...\n";
	int wsResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (wsResult != 0)
	{
		std::cerr << "\nWinsock setup failed! Error Code : " << WSAGetLastError() << '\n';
		return 1;
	}
	read_theaters();
	read_clients();
	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "\nSocket creation failed! Error Code : " << WSAGetLastError() << '\n';
		return 1;
	}
	std::cout << "\nSocket created.\n";
	// Bind the socket (ip address and port)
	SOCKADDR_IN hint{};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(DS_TEST_PORT);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listening, (SOCKADDR*)&hint, sizeof(hint));
	// Setup the socket for listening
	listen(listening, SOMAXCONN);
	// Wait for connection
	SOCKADDR_IN client_addr{};
	int clientSize = sizeof(client_addr);
	SOCKET clientSocket;
	std::list<std::thread> threads;
	int i = 0;
	while (i++ < MAX_THREADS && (clientSocket = accept(listening, (SOCKADDR*)&client_addr, &clientSize)) != SOCKET_ERROR)
	{
		threads.emplace_back(main_call, clientSocket, client_addr);
	}
	closesocket(listening);
	for (auto& thread : threads)
	{
		thread.join();
	}
	// Save data to files
	write_theaters();
	write_clients();
	// Cleanup winsock
	WSACleanup();
	return 0;
}
