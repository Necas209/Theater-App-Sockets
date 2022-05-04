#include "Theater.h"

Theater::Theater()
= default;

Theater::~Theater()
{
	shows.clear();
}

void to_json(json & j, const Theater & t)
{
	j = json{ {"name", t.name},
		{"location", t.location},
		{"shows", t.shows }
	};
}

void from_json(const json & j, Theater & t)
{
	j.at("name").get_to(t.name);
	j.at("location").get_to(t.location);
	j.at("shows").get_to(t.shows);
}
