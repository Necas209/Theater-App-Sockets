#include "theater_log.h"

theater_log::theater_log(const message& msg, const enum sender s, const std::string& ip)
{
	this->msg = msg;
	sender = s;
	ip_addr = ip;
}

std::string log_path(const theater_log & l)
{
	std::ostringstream ss;
	ss << R"(.\theater_logs\)" << std::put_time(&l.msg.stamp, "%Y-%m-%d") << ".Log";
	return ss.str();
}

std::ofstream& operator<<(std::ofstream & ofs, const theater_log & l)
{
	ofs << std::put_time(&l.msg.stamp, message::fmt_str) << ',';
	switch (l.sender)
	{
	case sender::client:
		ofs << "From:";
		break;
	case sender::server:
		ofs << "To:";
		break;
	}
	ofs << l.ip_addr << ',';
	ofs << codename.at(l.msg.code) << ',';
	ofs << l.msg.content << '\n';
	return ofs;
}
