#pragma once
#include <list>
#include "Show.h"

class Theater
{
public:
	std::string name;
	std::string location;
	std::list<Show> shows;
	Theater();
	Theater(std::string name, std::string location);
	virtual ~Theater();
	void write();
	void write_file(std::ofstream& ofs);
	friend void to_json(json& j, const Theater& t);
	friend void from_json(const json& j, Theater& t);
};