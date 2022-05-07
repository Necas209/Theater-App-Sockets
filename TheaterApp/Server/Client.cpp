#include "client.h"

client::client()
= default;

client::client(const std::string& ip_addr)
{
	this->ip_addr = ip_addr;
}

void to_json(json& j, const client& c) {
	j = json{ {"ip_addr", c.ip_addr},
		{"shows_seen", c.shows_seen},
	};
}

void from_json(const json& j, client& c) {
	j.at("ip_addr").get_to(c.ip_addr);
	j.at("shows_seen").get_to(c.shows_seen);
}