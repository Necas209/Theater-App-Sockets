#pragma once
#include <fstream>
#include <string>
#include <list>
#include "json.hpp"

using json = nlohmann::json;

class client final
{
public:
	std::string ip_addr;
	std::list<int> shows_seen;
	client();
	explicit client(const std::string& ip_addr);
	~client();
	bool has_seen(int id);
	friend void to_json(json& j, const client& c);
	friend void from_json(const json& j, client& c);
};