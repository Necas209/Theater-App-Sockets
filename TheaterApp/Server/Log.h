#pragma once
#include <fstream>
#include "Message.h"

const enum struct SENDER : int {
	CLIENT,
	SERVER
};


class Log
{
private:
	// attributes
	Message message;
	SENDER sender;
	std::string ip_addr;
public:
	// methods
	Log(Message msg, SENDER s, std::string ip);
	virtual ~Log();
	friend std::string log_path(const Log& l);
	friend std::ofstream& operator<<(std::ofstream& ofs, const Log& l);
};

