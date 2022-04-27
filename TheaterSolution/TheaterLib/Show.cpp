#include "Show.h"

Show::Show(int id, std::string name, tm datetime, int capacity, int available_seats)
{
	this->id = id;
	this->name = name;
	this->datetime = datetime;
	this->capacity = capacity;
	this->available_seats = available_seats;
}

Show::~Show()
{
}

void Show::write()
{
	std::cout << name << "; " << std::put_time(&datetime, "%c") << "; ";
	std::cout << capacity << "; " << available_seats << "; ";
}

void Show::write_file(std::ofstream& ofs)
{
	ofs << id << ',' << name << ',';
	ofs << std::put_time(&datetime, "%c") << ',';
	ofs << ',' << capacity << ',' << available_seats;
}

void to_json(json& j, const Show& s)
{
	std::ostringstream ss;
	ss << std::put_time(&s.datetime, "%c");
	j = json{ {"id", s.id},
		{"name", s.name},
		{"datetime", ss.str()},
		{"capacity", s.capacity},
		{"available_seats", s.available_seats}
	};
}

void from_json(const json& j, Show& s)
{
	std::string aux;
	j.at("id").get_to(s.id);
	j.at("name").get_to(s.name);
	j.at("datetime").get_to(aux);
	std::istringstream ss{ aux };
	ss >> std::get_time(&s.datetime, "%a %b %d %H:%M:%S %Y");
	j.at("capacity").get_to(s.capacity);
	j.at("available_seats").get_to(s.available_seats);
}