#pragma once
#include <fstream>
#include <string>
#include <list>
#include "json.hpp"

using json = nlohmann::json;

class Client
{
public:
	std::string ip_addr;
	std::list<int> showsSeen;
	std::list<int> showsRec;
	Client();
	Client(std::string ip_addr);
	virtual ~Client();
	bool been_recommended(const int id);
	friend void to_json(json& j, const Client& c);
	friend void from_json(const json& j, Client& c);
};