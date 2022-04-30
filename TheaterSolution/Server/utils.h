#pragma once
#include <sstream>
#include <set>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Theater.h"
#include "Client.h"
#include "Message.h"

#pragma comment(lib, "ws2_32.lib")

extern std::list<Theater> theaters;
extern std::map<std::string, Client> clients;
extern std::mutex tickets_mutex;

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
 * @brief Sends available locations to client
 * @param clientSocket: client socket
 * @return integer: value of send
*/
int sendLocations(SOCKET& clientSocket);
/**
 * @brief Sends available genres to client
 * @param clientSocket: client socket
 * @param msg: last message received
 * @return integer: value of send
*/
int sendGenres(SOCKET& clientSocket, Message& msg);
/**
 * @brief Sends available shows to client, given location and genre
 * @param clientSocket: client socket
 * @param msg: last message received
 * @param ip_addr: client's IP address
 * @param it: iterator pointing to chosen theater
 * @return integer: value of send
*/
int sendShows(SOCKET& clientSocket, Message& msg, const std::string& ip_addr, std::list<Theater>::iterator& it);
/**
 * @brief Processes client's ticket purchase
 * @param clientSocket: client socket
 * @param msg: last message received
 * @param ip_addr: client's IP address
 * @return 
*/
int buyTickets(SOCKET& clientSocket, Message& msg, const std::string& ip_addr);
/**
* @brief Main client call function
* @param clientSocket: client socket
* @param client: socket address
* @return integer: the call state
*/
int ClientCall(SOCKET clientSocket, SOCKADDR_IN client);
/**
 * @brief Quits call with client
 * @param clientSocket: client socket
 * @return integer: send return value
*/
int quitCall(SOCKET& clientSocket);