#pragma once
#include <fstream>
#include <string>
#include <list>
#include "json.hpp"

using json = nlohmann::json;

class Client final
{
public:
	std::string ip_addr;
	std::list<int> shows_seen;
	std::list<int> shows_rec;
	Client();
	explicit Client(const std::string& ip_addr);
	~Client();
	bool been_recommended(int id);
	friend void to_json(json& j, const Client& c);
	friend void from_json(const json& j, Client& c);
};