#include "client.h"

client::client()
= default;

client::client(const std::string& ip_addr)
{
	this->ip_addr = ip_addr;
}

bool client::has_seen(const int id)
{
	const auto it = std::ranges::find_if(shows_seen,
		[&](const auto& p) { return p.first == id; });
	return it != shows_seen.end();
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