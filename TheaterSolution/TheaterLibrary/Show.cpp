#include "Show.h"

Show::Show()
{
	//time_t t;
	//time(&t);
	//localtime_s(&datetime, &t);
	capacity = 0;
	available_seats = 0;
}

Show::Show(int id, std::string name, std::string genre, tm datetime, int capacity, int available_seats)
{
	this->id = id;
	this->name = name;
	this->genre = genre;
	this->datetime = datetime;
	this->capacity = capacity;
	this->available_seats = available_seats;
}

Show::~Show()
{
}

void Show::write()
{
	std::cout << '(' << id << ") -> ";
	std::cout << name << " | " << genre << " | ";
	std::cout << std::put_time(&datetime, fmt_str) << '\n';
	std::cout << "Capacity: " << capacity << '\n';
	std::cout << "Available seats: " << available_seats << '\n';
}

void Show::write_file(std::ofstream& ofs)
{
	ofs << id << ',' << name << ',' << genre << ',';
	ofs << std::put_time(&datetime, fmt_str) << ',';
	ofs << ',' << capacity << ',' << available_seats;
}

void to_json(json& j, const Show& s)
{
	std::ostringstream ss;
	ss << std::put_time(&s.datetime, Show::fmt_str);
	j = json{ {"id", s.id},
		{"name", s.name},
		{"genre", s.genre},
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
	j.at("genre").get_to(s.genre);
	j.at("datetime").get_to(aux);
	std::istringstream ss{ aux };
	ss >> std::get_time(&s.datetime, Show::fmt_str);
	j.at("capacity").get_to(s.capacity);
	j.at("available_seats").get_to(s.available_seats);
}