#pragma once
#include <string>
#include <map>
#include "json.hpp"

using json = nlohmann::json;

class client final
{
public:
	std::string ip_addr;
	/**
	 * @brief map of purchases: show id -> number of tickets
	*/
	std::map<int, int> shows_seen;
	client();
	explicit client(const std::string& ip_addr);
	friend void to_json(json& j, const client& c);
	friend void from_json(const json& j, client& c);
};
