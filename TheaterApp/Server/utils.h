#pragma once
#include <fstream>
#include <mutex>
#include <set>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "client.h"
#include "message.h"
#include "theater.h"

#pragma comment(lib, "ws2_32.lib")

extern std::list<theater> theaters;
extern std::map<std::string, client> clients;
extern std::mutex tickets_mutex;
extern std::mutex log_mutex;
extern thread_local std::string ip_addr;

enum struct sender
{
	client,
	server
};
/**
 * @brief Logs message
 * @param msg: message send/received
 * @param sender: CLIENT or SERVER
*/
void log_message(const message& msg, sender sender);
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
 * @param client_socket: client socket
 * @return integer: value of send
*/
int get_locations(const SOCKET& client_socket);
/**
 * @brief Sends available genres to client
 * @param client_socket: client socket
 * @param location: location received
 * @return integer: value of send
*/
int get_genres(const SOCKET& client_socket, const std::string& location);
/**
 * @brief Sends available shows to client, given location and genre
 * @param client_socket: client socket
 * @param content: message content
 * @param it: iterator pointing to chosen theater
 * @return integer: value of send
*/
int get_shows(const SOCKET& client_socket, const std::string& content, std::list<theater>::iterator& it);
/**
 * @brief Processes client's ticket purchase
 * @param client_socket: client socket
 * @param content: last message content
 * @return
*/
int buy_tickets(const SOCKET& client_socket, const std::string& content);
/**
* @brief Main client call function
* @param client_socket: client socket
* @param client_addr: socket address
* @return integer: the call state
*/
int main_call(SOCKET client_socket, sockaddr_in client_addr);
/**
 * @brief Quits call with client
 * @param client_socket: client socket
 * @return integer: send return value
*/
int quit_call(const SOCKET& client_socket);