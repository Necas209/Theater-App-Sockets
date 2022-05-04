#pragma once
#include <fstream>
#include "Message.h"

enum struct SENDER {
	CLIENT,
	SERVER
};


class Log final
{
public:
	// attributes
	Message message;
	SENDER sender;
	std::string ip_addr;
	// methods
	Log(const Message& msg, SENDER s, const std::string& ip);
	~Log();
	friend std::string log_path(const Log& l);
	friend std::ofstream& operator<<(std::ofstream& ofs, const Log& l);
};

