#pragma once
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <regex>
#include <WinSock2.h>
#include "Message.h"
#include "Show.h"

#pragma comment(lib, "ws2_32.lib")

extern std::list<Show> shows;

/**
 * @brief Main server call function
 * @param serverSocket: server socket
 * @return integer: the call state
*/
int main_call(SOCKET& serverSocket);
/**
 * @brief Validates IP address
 * @param ip_addr: IP address
 * @return boolean: whether IP address is valid or not
*/
bool validate_ip(const std::string& ip_addr);
/**
 * @brief Asks client for location, and
 * receives confirmation from server
 * @param serverSocket: server socket
 * @param location: string to store location in
 * @return integer: 0/1 if server received location,
	SOCKET_ERROR if any error occurred
*/
int pick_location(SOCKET& serverSocket, std::string& location);
/**
 * @brief Asks client for show genre, and
 * receives number of available shows from server
 * @param serverSocket: server socket
  * @param genre: string to store genre in
 * @return integer: number of available shows, otherwise SOCKET_ERROR
*/
int pick_genre(SOCKET& serverSocket, std::string& location, std::string& genre);
/**
 * @brief Asks client to pick show and number of tickets
 * @param serverSocket: server socket
 * @param location: location picked by client
 * @return pair of integers: show ID and number of tickets
*/
std::pair<int, int> pick_show(SOCKET& serverSocket);
/**
* @brief Asks client if he wishes to quit the call
* @param serverSocket: server socket
* @return integer: 0 if client wishes to continue
*/
int quit_call(SOCKET& serverSocket);