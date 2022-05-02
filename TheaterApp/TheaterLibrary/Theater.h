#pragma once
#include <list>
#include "Show.h"

class Theater
{
public:
	// attributes
	std::string name;
	std::string location;
	std::list<Show> shows;
	// methods
	Theater();
	virtual ~Theater();
	void write();
	// serializer and deserializer
	friend void to_json(json& j, const Theater& t);
	friend void from_json(const json& j, Theater& t);
};