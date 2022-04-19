#include "Theater.h"

Theater::Theater(std::string name, std::string location)
	: name(name), location(location)
{
}

Theater::~Theater()
{
	for (auto& show : shows)
	{
		delete show;
	}
	shows.clear();
}

void Theater::Write()
{
	std::cout << name << "; " << location << "; ";
	for (const auto& show : shows)
	{
		show->Write();
	}
}

void Theater::WriteFile(std::ofstream& ofs)
{
	for (const auto& show : shows)
	{
		ofs << name << ',' << location << ',';
		show->WriteFile(ofs);
		ofs << '\n';
	}
}
