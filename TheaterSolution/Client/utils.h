#pragma once
#include <iostream>
#include <string>
#include <regex>
#include <WinSock2.h>

int ServerCall(SOCKET& s);
bool validateIP(const std::string& ip_addr);