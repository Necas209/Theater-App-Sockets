#pragma once

#include <fstream>
#include <string>
#include <list>
#include "json.hpp"

using json = nlohmann::json;

class Client
{
public:
	std::string ip;
	std::list<int> showsSeen;
	std::list<int> showsRecommended;
	Client();
	Client(std::string ip);
	virtual ~Client();
};

void to_json(json& j, const Client& c);
void from_json(const json& j, Client& c);