#include "Log.h"

Log::Log(const Message& msg, const SENDER s, const std::string& ip)
{
	message = msg;
	sender = s;
	ip_addr = ip;
}

Log::~Log()
= default;

std::string log_path(const Log & l)
{
	std::ostringstream ss;
	ss << R"(.\theater_logs\)" << std::put_time(&l.message.stamp, "%Y-%m-%d") << ".log";
	return ss.str();
}

std::ofstream& operator<<(std::ofstream & ofs, const Log & l)
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
