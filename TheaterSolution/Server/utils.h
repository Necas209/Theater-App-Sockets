#pragma once
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Theater.h"
#include "Client.h"

#pragma comment(lib, "ws2_32.lib")

extern std::list<Theater> theaters;
extern std::map<std::string, Client> clients;

/**
 * @brief Writes theaters to console
*/
void WriteTheaters();
/**
 * @brief Reads theaters from file
 * @param filename: path to JSON file
*/
void ReadTheatersFromFile(const char* filename = "shows.json");
/**
 * @brief Writes theaters to file
 * @param filename: path to JSON file
*/
void WriteTheatersToFile(const char* filename = "shows.json");
/**
 * @brief Reads clients from file
 * @param filename: path to JSON file
*/
void ReadClientsFromFile(const char* filename = "clients.json");
/**
 * @brief Writes clients to file
 * @param filename: path to JSON file
*/
void WriteClientsToFile(const char* filename = "clients.json");
/**
* @brief Main client call function
* @param clientSocket: client socket
* @param client: socket address
* @return integer: the call state
*/
int ClientCall(SOCKET clientSocket, SOCKADDR_IN client);