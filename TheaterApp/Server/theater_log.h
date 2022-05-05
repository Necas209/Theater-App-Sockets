#pragma once
#include <fstream>
#include "message.h"

enum struct sender {
	client,
	server
};


class theater_log final
{
public:
	// attributes
	message message;
	sender sender;
	std::string ip_addr;
	// methods
	theater_log(const class message& msg, enum sender s, const std::string& ip);
	~theater_log();
	friend std::string log_path(const theater_log& l);
	friend std::ofstream& operator<<(std::ofstream& ofs, const theater_log& l);
};

