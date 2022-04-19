#include "Client.h"

Client::Client()
{
}

Client::Client(std::string ip)
	:ip(ip)
{
}

Client::~Client()
{
	showsSeen.clear();
}

void to_json(json& j, const Client& c) {
	j = json{ {"ip", c.ip},
		{"showsSeen", c.showsSeen},
		{"showsRecommended", c.showsRecommended}
	};
}

void from_json(const json& j, Client& c) {
	j.at("ip").get_to(c.ip);
	j.at("showsSeen").get_to(c.showsSeen);
	j.at("showsRecommended").get_to(c.showsRecommended);
}