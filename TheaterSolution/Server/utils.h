#pragma once
#include <sstream>
#include <map>
#include <WinSock2.h>
#include "Theater.h"
#include "Client.h"
#include "csv.h"

extern std::map<std::string, Theater*> theaters;
extern std::map<std::string, Client> clients;

void WriteTheaters();
void ReadTheatersFromFile(const char* filename = "shows.csv");
void WriteTheatersToFile(const char* filename = "shows.csv");
void ReadClientsFromFile(const char* filename = "clients.json");
void WriteClientsToFile(const char* filename = "clients.json");
int ClientCall(SOCKET clientSocket, SOCKADDR_IN client);