#include "utils.h"

std::map<std::string, Theater*> theaters;

void WriteTheaters()
{
	for (const auto& [name, theater] : theaters)
	{
		theater->Write();
	}
}

void ReadTheatersFromFile(const char* filename)
{
	io::CSVReader<6> reader(filename);
	reader.read_header(io::ignore_no_column,
		"theater", "location", "name", "datetime", "capacity", "available_seats");

	std::string theater_name, location, name, datetime;
	int capacity, available_seats;

	while (reader.read_row(theater_name, location, name,
		datetime, capacity, available_seats))
	{
		if (!theaters.contains(theater_name))
		{
			Theater* theater = new Theater(name, location);
			theaters.insert(std::make_pair(theater_name, theater));
		}
		Show* show = new Show(name, capacity, available_seats);
		std::istringstream ss{ datetime };
		ss >> std::get_time(&show->datetime, "%a %b %d %H:%M:%S %Y");
		theaters[theater_name]->shows.push_back(show);
	}
}

void WriteTheatersToFile(const char* filename)
{
	std::ofstream ofs{ filename };
	ofs << "theater,location,name,datetime,capacity,available_seats\n";
	for (const auto& [name, theater] : theaters)
	{
		theater->WriteFile(ofs);
	}
}

int ClientCall(SOCKET clientSocket)
{
	char strMsg[1024];
	char strRec[1024];

	int i = 1;

	strcpy(strMsg, "100 OK");
	printf("\n%s\n", strMsg);
	send(clientSocket, strMsg, (int)strlen(strMsg) + 1, 0);

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
			break;
		}

		printf("%i : %s\n", i++, strRec);
		if (strcmp(strupr(strRec), "QUIT") == 0)
		{
			strcpy(strMsg, "400 BYE");
			send(clientSocket, strMsg, (int)strlen(strMsg) + 1, 0);
			std::cout << "Bye, client...\n\n";
			break;
		}
		else
		{
			send(clientSocket, strRec, bytesReceived + 1, 0);
		}
	}
	// Close the socket
	return closesocket(clientSocket);
}