#include "Show.h"

Show::Show()
{
	datetime = new tm{};
	capacity = 0;
	available_seats = 0;
}

Show::~Show()
{
}

void Show::Write()
{
	char aux[100];
	strftime(aux, sizeof(aux), "%c", datetime);
	std::cout << name << "; " << aux << "; ";
	std::cout << capacity << "; " << available_seats << "; ";
}

void Show::WriteFile(std::ofstream& ofs)
{
	char buf[100];
	ofs << '\"' << name << "\",";
	strftime(buf, sizeof(buf), "%c", datetime);
	ofs << '\"' << buf << "\",";
	ofs << capacity << ',' << available_seats;
}
