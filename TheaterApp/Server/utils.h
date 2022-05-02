#pragma once
#include <set>
#include <mutex>
#include <filesystem>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Theater.h"
#include "Client.h"
#include "Log.h"

#pragma comment(lib, "ws2_32.lib")

extern std::list<Theater> theaters;
extern std::map<std::string, Client> clients;
extern std::mutex tickets_mutex;
extern std::mutex log_mutex;
extern thread_local std::string ip_addr;

using theater_it = std::list<Theater>::iterator;

/**
 * @brief Logs message
 * @param msg: message send/received
 * @param sender: CLIENT or SERVER
*/
void log_message(Message msg, SENDER sender);
/**
 * @brief Reads theaters from file
 * @param filename: path to JSON file
*/
void read_theaters(const char* filename = "shows.json");
/**
 * @brief Writes theaters to file
 * @param filename: path to JSON file
*/
void write_theaters(const char* filename = "shows.json");
/**
 * @brief Reads clients from file
 * @param filename: path to JSON file
*/
void read_clients(const char* filename = "clients.json");
/**
 * @brief Writes clients to file
 * @param filename: path to JSON file
*/
void write_clients(const char* filename = "clients.json");
/**
 * @brief Sends available locations to client
 * @param clientSocket: client socket
 * @return integer: value of send
*/
int get_locations(SOCKET& clientSocket);
/**
 * @brief Sends available genres to client
 * @param clientSocket: client socket
 * @param msg: last message received
 * @return integer: value of send
*/
int get_genres(SOCKET& clientSocket, Message& msg);
/**
 * @brief Sends available shows to client, given location and genre
 * @param clientSocket: client socket
 * @param msg: last message received
 * @param it: iterator pointing to chosen theater
 * @return integer: value of send
*/
int get_shows(SOCKET& clientSocket, Message& msg, theater_it& it);
/**
 * @brief Processes client's ticket purchase
 * @param clientSocket: client socket
 * @param msg: last message received
 * @return 
*/
int buy_tickets(SOCKET& clientSocket, Message& msg);
/**
* @brief Main client call function
* @param clientSocket: client socket
* @param client_addr: socket address
* @return integer: the call state
*/
int main_call(SOCKET clientSocket, SOCKADDR_IN client_addr);
/**
 * @brief Quits call with client
 * @param clientSocket: client socket
 * @return integer: send return value
*/
int quit_call(SOCKET& clientSocket);