#include "Client.h"

Client::Client()
= default;

Client::Client(const std::string & ip_addr)
{
	this->ip_addr = ip_addr;
}

Client::~Client()
{
	shows_seen.clear();
	shows_rec.clear();
}

bool Client::been_recommended(const int id)
{
	const auto it = std::ranges::find_if(shows_rec,
		[&](const int show_id) { return show_id == id; });
	return it != shows_rec.end();
}

void to_json(json & j, const Client & c) {
	j = json{ {"ip_addr", c.ip_addr},
		{"shows_seen", c.shows_seen},
		{"shows_rec", c.shows_rec}
	};
}

void from_json(const json & j, Client & c) {
	j.at("ip_addr").get_to(c.ip_addr);
	j.at("shows_seen").get_to(c.shows_seen);
	j.at("shows_rec").get_to(c.shows_rec);
}