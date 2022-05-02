#include "Message.h"

Message::Message()
{
}

Message::Message(CODE code, std::string content)
{
	this->code = code;
	this->content = content;
}

Message::~Message()
{
}

void to_json(json& j, const Message& m)
{
	j = json{ {"code", m.code},
		{"content", m.content} };
}

void from_json(const json& j, Message& m)
{
	j.at("code").get_to(m.code);
	j.at("content").get_to(m.content);
}
