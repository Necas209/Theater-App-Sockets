#include "utils.h"
#include <thread>

constexpr auto DS_TEST_PORT = (u_short)68000;
//constexpr auto MAX_THREADS = 10;

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);
	// Initialise winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	std::cout << "Initialising Winsock...\n";
	int wsResult = WSAStartup(ver, &wsData);
	if (wsResult != 0)
	{
		fprintf(stderr, "\nWinsock setup failed! Error Code : %d\n", WSAGetLastError());
		return 1;
	}
	ReadTheatersFromFile();
	WriteTheaters();
	ReadClientsFromFile();
	std::cout << '\n';

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		fprintf(stderr, "\nSocket creation failed! Error Code : %d\n", WSAGetLastError());
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
	SOCKADDR_IN client{};
	int clientSize = sizeof(client);

	// Client Communication
	SOCKET clientSocket;
	std::list<std::thread> threads;
	while ((clientSocket = accept(listening, (SOCKADDR*)&client, &clientSize)) != SOCKET_ERROR)
	{
		threads.emplace_back(ClientCall, clientSocket, client);
	}
	closesocket(listening);
	for (auto& thread : threads)
	{
		thread.join();
	}

	// Save data to files
	WriteTheatersToFile();
	WriteClientsToFile();

	// Cleanup winsock
	WSACleanup();
	return 0;
}
