/*
	Simple winsock Server
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>

#define DS_TEST_PORT 68000

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

int main()
{
	// Initialise winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	printf("\nInitialising Winsock...");
	int wsResult = WSAStartup(ver, &wsData);
	if (wsResult != 0)
	{
		fprintf(stderr, "\nWinsock setup fail! Error Code : %d\n", WSAGetLastError());
		return 1;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		fprintf(stderr, "\nSocket creationg fail! Error Code : %d\n", WSAGetLastError());
		return 1;
	}

	printf("\nSocket created.");

	// Bind the socket (ip address and port)
	struct sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(DS_TEST_PORT);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (struct sockaddr*)&hint, sizeof(hint));

	// Setup the socket for listening
	listen(listening, SOMAXCONN);

	// Wait for connection
	struct sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (struct sockaddr*)&client, &clientSize);

	// Main program loop
	char strMsg[1024];
	char strRec[1024];

	int i = 1;

	strcpy(strMsg, "\nHello! welcome to the server...\n");
	printf("\n%s\n", strMsg);
	send(clientSocket, strMsg, strlen(strMsg) + 1, 0);

	while (true)
	{
		ZeroMemory(strRec, 1024);
		int bytesReceived = recv(clientSocket, strRec, 1024, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			printf("\nReceive error!\n");
			break;
		}
		if (bytesReceived == 0)
		{
			printf("\nClient disconnected!\n");
			clientSocket = accept(listening, (struct sockaddr*)&client, &clientSize);
		}

		printf("%i : %s\n", i++, strRec);

		if (strcmp(strRec, "date") == 0)
		{
			// current date/time based on current system
			time_t now = time(0);
			// convert now to string form
			char *dt = ctime(&now);

			strcpy(strMsg, "\n\nThe local date and time is: ");
			strcat(strMsg, dt);
			strcat(strMsg, "\n");

			send(clientSocket, strMsg, strlen(strMsg) + 1, 0);
		}
		else if (strcmp(strRec, "bye") == 0)
		{
			strcpy(strMsg, "\nBye client...\n");
			send(clientSocket, strMsg, strlen(strMsg) + 1, 0);
			clientSocket = accept(listening, (struct sockaddr*)&client, &clientSize);

			strcpy(strMsg, "\nHello! welcome to the server...\n");
			printf("\n%s\n", strMsg);
			send(clientSocket, strMsg, strlen(strMsg) + 1, 0);
		}
		else
		{
			send(clientSocket, strRec, bytesReceived + 1, 0);
		}
	}

	// Close listening socket
	closesocket(listening);

	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();

	return 0;
}
