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
	std::string get_location()
	{
		std::string loc(location);
		std::transform(loc.begin(), loc.end(), loc.begin(), toupper);
		return loc;
	};
	void write();
	void write_file(std::ofstream& ofs);
	friend void to_json(json& j, const Theater& t);
	friend void from_json(const json& j, Theater& t);
};