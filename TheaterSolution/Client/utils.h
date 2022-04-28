#pragma once
#include <iostream>
#include <string>
#include <list>
#include <regex>
#include <WinSock2.h>
#include "Show.h"

int ServerCall(SOCKET& s);
bool validateIP(const std::string& ip_addr);
int askLocation(SOCKET& serverSocket);
int askGenre(SOCKET& serverSocket);
bool quitCall(SOCKET& serverSocket);