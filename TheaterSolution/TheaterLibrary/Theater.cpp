#include "Theater.h"

Theater::Theater()
{
}

Theater::Theater(std::string name, std::string location)
	: name(name), location(location)
{
}

Theater::~Theater()
{
	shows.clear();
}

void Theater::write()
{
	std::cout << name << "; " << location << "; ";
	for (auto& show : shows)
	{
		show.write();
	}
}

void Theater::write_file(std::ofstream& ofs)
{
	for (auto& show : shows)
	{
		ofs << name << ',' << location << ',';
		show.write_file(ofs);
		ofs << '\n';
	}
}

void to_json(json& j, const Theater& t)
{
	j = json{ {"name", t.name},
		{"location", t.location},
		{"shows", t.shows }
	};
}

void from_json(const json& j, Theater& t)
{
	j.at("name").get_to(t.name);
	j.at("location").get_to(t.location);
	j.at("shows").get_to(t.shows);
}
