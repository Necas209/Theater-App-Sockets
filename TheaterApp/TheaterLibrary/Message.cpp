#include "Message.h"

const std::map<CODE, const char*> codename
{
	{CODE::HELLO, "HELLO"},
	{CODE::GET_LOCATIONS, "GET_LOCATIONS"},
	{CODE::GET_GENRES, "GET_GENRES"},
	{CODE::GET_SHOWS, "GET_SHOWS"},
	{CODE::BUY_TICKETS, "BUY_TICKETS"},
	{CODE::QUIT, "QUIT"}
};

Message::Message()
{
}

Message::Message(CODE code, std::string content)
{
	this->code = code;
	this->content = content;
	time_t t;
	time(&t);
	localtime_s(&stamp, &t);
}

Message::~Message()
{
}

void to_json(json& j, const Message& m)
{
	std::ostringstream ss;
	ss << std::put_time(&m.stamp, Message::fmt_str);
	j = json{ {"code", m.code},
		{"content", m.content},
		{"stamp", ss.str()} };
}

void from_json(const json& j, Message& m)
{
	j.at("code").get_to(m.code);
	j.at("content").get_to(m.content);
	std::istringstream ss{ j.at("stamp").get<std::string>() };
	ss >> std::get_time(&m.stamp, Message::fmt_str);
}
