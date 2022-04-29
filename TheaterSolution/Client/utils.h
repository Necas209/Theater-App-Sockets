#pragma once
#include <iostream>
#include <string>
#include <list>
#include <regex>
#include <WinSock2.h>
#include "Show.h"

#pragma comment(lib, "ws2_32.lib")

enum { ERR = -1, FAIL, SUCCESS };

extern std::list<Show> shows;

/**
 * @brief Main server call function
 * @param serverSocket: server socket
 * @return integer: the call state
*/
int ServerCall(SOCKET& serverSocket);
/**
 * @brief Validates IP address
 * @param ip_addr: IP address
 * @return boolean: whether IP address is valid or not
*/
bool validateIP(const std::string& ip_addr);
/**
 * @brief Asks client for location, and
 * receives confirmation from server
 * @param serverSocket: server socket
 * @return integer: 0/1 if server received location,
	SOCKET_ERROR if any error occurred
*/
int askLocation(SOCKET& serverSocket);
/**
 * @brief Asks client for show genre, and
 * receives number of available shows from server
 * @param serverSocket: server socket
 * @return integer: number of available shows, otherwise SOCKET_ERROR
*/
int askGenre(SOCKET& serverSocket);
/**
* @brief Asks client if he wishes to quit the call
* @param serverSocket: server socket
* @return integer: 0 if client wishes to continue
*/
int quitCall(SOCKET& serverSocket);