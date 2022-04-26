#include "Theater.h"

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
