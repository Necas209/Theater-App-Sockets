#include "Client.h"

Client::Client()
{
}

Client::Client(std::string ip_addr)
{
	this->ip_addr = ip_addr;
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
	j = json{ {"ip_addr", c.ip_addr},
		{"showsSeen", c.showsSeen},
		{"showsRec", c.showsRec}
	};
}

void from_json(const json& j, Client& c) {
	j.at("ip_addr").get_to(c.ip_addr);
	j.at("showsSeen").get_to(c.showsSeen);
	j.at("showsRec").get_to(c.showsRec);
}