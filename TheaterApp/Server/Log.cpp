#include "Log.h"

Log::Log(Message msg, SENDER s, std::string ip)
{
	message = msg;
	sender = s;
	ip_addr = ip;
}

Log::~Log()
{
}

std::string log_path(const Log& l)
{
	std::ostringstream ss;
	ss << std::put_time(&l.message.stamp, "%F");
	return "./logs/" + ss.str() + ".log";
}

std::ofstream& operator<<(std::ofstream& ofs, const Log& l)
{
	ofs << std::put_time(&l.message.stamp, Message::fmt_str) << ',';
	switch (l.sender)
	{
	case SENDER::CLIENT:
		ofs << "From:";
		break;
	case SENDER::SERVER:
		ofs << "To:";
		break;
	}
	ofs << l.ip_addr << ',';
	ofs << codename.at(l.message.code) << ',';
	ofs << l.message.content << '\n';
	return ofs;
}
