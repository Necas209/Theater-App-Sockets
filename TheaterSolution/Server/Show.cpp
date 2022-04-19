#include "Show.h"

Show::Show(std::string name, int capacity, int available_seats)
	: name(name), capacity(capacity), available_seats(available_seats)
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
	ofs << name << ',' << std::put_time(&datetime, "%c") << ',';
	ofs << ',' << capacity << ',' << available_seats;
}
