#include "Show.h"

Show::Show(int id, std::string name, int capacity, int available_seats)
	: id(id), name(name), capacity(capacity), available_seats(available_seats)
{
}

Show::~Show()
{
}

void Show::Write()
{
	std::cout << name << "; " << std::put_time(&datetime, "%c") << "; ";
	std::cout << capacity << "; " << available_seats << "; ";
}

void Show::WriteFile(std::ofstream& ofs)
{
	ofs << id << ',' << name << ',';
	ofs << std::put_time(&datetime, "%c") << ',';
	ofs << ',' << capacity << ',' << available_seats;
}
