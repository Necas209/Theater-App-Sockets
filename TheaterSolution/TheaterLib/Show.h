#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include "json.hpp"

using json = nlohmann::json;

class Show
{
public:
	int id;
	std::string name;
	tm datetime;
	int capacity;
	int available_seats;

	Show(int id, std::string name, tm datetime, int capacity, int available_seats);
	virtual ~Show();
	void write();
	void write_file(std::ofstream& ofs);
};

void to_json(json& j, const Show& s);
void from_json(const json& j, Show& s);