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

bool Client::been_recommended(const int id)
{
	auto it = std::find_if(showsRec.begin(), showsRec.end(), [&](int show_id) { return show_id == id; });
	return it != showsRec.end();
}

void to_json(json& j, const Client& c) {
	j = json{ {"ip", c.ip},
		{"showsSeen", c.showsSeen},
		{"showsRecommended", c.showsRec}
	};
}

void from_json(const json& j, Client& c) {
	j.at("ip").get_to(c.ip);
	j.at("showsSeen").get_to(c.showsSeen);
	j.at("showsRecommended").get_to(c.showsRec);
}