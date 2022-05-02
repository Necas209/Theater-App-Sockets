#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include "json.hpp"

using json = nlohmann::json;

class Show
{
private:
	static constexpr char fmt_str[] = "%a %b %d %H:%M:%S %Y";
public:
	int id;
	std::string name;
	std::string genre;
	tm datetime;
	int capacity;
	int available_seats;
	Show();
	virtual ~Show();
	void write();
	friend void to_json(json& j, const Show& s);
	friend void from_json(const json& j, Show& s);
};