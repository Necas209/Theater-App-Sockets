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
	virtual ~Theater();
	void write();
	friend void to_json(json& j, const Theater& t);
	friend void from_json(const json& j, Theater& t);
};