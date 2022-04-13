#include "Show.h"

Show::Show()
{
	capacity = 0;
	available_seats = 0;
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
