/*
	Simple winsock client
*/

#include <stdbool.h>
#include <stdio.h>
#include <winsock2.h>

#pragma warning(disable : 4996)

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	SOCKADDR_IN server;
	char message[2000], server_reply[2000];
	int recv_size;
	int ws_result;

	// Initialise winsock
	printf("\nInitialising Winsock...");
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

	printf("Socket created.\n");

	// create the socket  address (ip address and port)
	server.sin_addr.s_addr = inet_addr("25.63.23.179");
	server.sin_family = AF_INET;
	server.sin_port = htons(68000);

	// Connect to remote server
	ws_result = connect(s, (SOCKADDR*)&server, sizeof(server));
	if (ws_result < 0)
	{
		puts("connect error");
		return 1;
	}

	puts("Connected");

	int i = 1;

	// Receive a reply from the server
	recv_size = recv(s, server_reply, 2000, 0);
	puts(server_reply);

	while (true)
	{
		// Send some data
		printf("Message %i : ", i);
		scanf("%[^\n]%*c", message);
		ws_result = send(s, message, (int)strlen(message) + 1, 0);
		if (ws_result < 0)
		{
			puts("Send failed");
			return 1;
		}
		puts("Data Sent\n");

		// Receive a reply from the server
		recv_size = recv(s, server_reply, 2000, 0);
		if (recv_size == SOCKET_ERROR)
		{
			puts("recv failed");
		}
		if (strcmp(server_reply, "400 BYE") == 0)
		{
			puts("Bye, server...\n");
			break;
		}
		puts("Reply received\n");
		printf("Reply %i : %s\n", i++, server_reply);
	}

	// Close the socket
	closesocket(s);

	// Cleanup winsock
	WSACleanup();

	return 0;
}
