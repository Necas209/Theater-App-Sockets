#pragma once
#include <iostream>
#include <list>
#include <regex>
#include <set>
#include <string>
#include <WinSock2.h>
#include "message.h"
#include "show.h"

#pragma comment(lib, "ws2_32.lib")

extern std::list<show> shows;

/**
 * @brief Main server call function
 * @param server_socket: server socket
 * @return integer: the call state
*/
int main_call(const SOCKET& server_socket);
/**
 * @brief Validates IP address
 * @param ip_address: IP address
 * @return boolean: whether IP address is valid or not
*/
bool validate_ip(const std::string& ip_address);
/**
 * @brief Asks client for location
 * @param server_socket: server socket
 * @param location: string to store location in
 * @return integer: call state
*/
int pick_location(const SOCKET& server_socket, std::string& location);
/**
 * @brief Asks client for show genre
 * @param server_socket: server socket
 * @param location: string containing location picked
 * @param genre: string to store genre in
 * @return integer: call state
*/
int pick_genre(const SOCKET& server_socket, const std::string& location, std::string& genre);
/**
 * @brief Asks client to pick show and number of tickets
 * @return pair of integers: show ID and number of tickets
*/
std::pair<int, int> pick_show();
/**
 * @brief Buy tickets
 * @param server_socket: server socket
 * @return integer: call state
*/
int buy_tickets(const SOCKET& server_socket);
/**
 * @brief Asks client if he wishes to quit the call
 * @param server_socket: server socket
 * @return integer: 0 if client wishes to continue
*/
int quit_call(const SOCKET& server_socket);