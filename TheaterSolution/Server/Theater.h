#pragma once
#include <list>
#include "Show.h"

class Theater
{
public:
	std::string name;
	std::string location;
	std::list<Show*> shows;

	Theater(std::string name, std::string location);
	virtual ~Theater();
	void Write();
	void WriteFile(std::ofstream& ofs);
};