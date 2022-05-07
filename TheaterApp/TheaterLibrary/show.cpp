#include "show.h"

show::show()
= default;

void show::write() const
{
	std::cout << '(' << id << ") -> ";
	std::cout << name << " | " << genre << " | ";
	std::cout << std::put_time(&datetime, fmt_str) << '\n';
	std::cout << "Capacity: " << capacity << '\n';
	std::cout << "Available seats: " << available_seats << '\n';
}

void to_json(json & j, const show & s)
{
	std::ostringstream ss;
	ss << std::put_time(&s.datetime, show::fmt_str);
	j = json{ {"id", s.id},
		{"name", s.name},
		{"genre", s.genre},
		{"datetime", ss.str()},
		{"capacity", s.capacity},
		{"available_seats", s.available_seats}
	};
}

void from_json(const json & j, show & s)
{
	j.at("id").get_to(s.id);
	j.at("name").get_to(s.name);
	j.at("genre").get_to(s.genre);
	std::istringstream ss{ j.at("datetime").get<std::string>() };
	ss >> std::get_time(&s.datetime, show::fmt_str);
	j.at("capacity").get_to(s.capacity);
	j.at("available_seats").get_to(s.available_seats);
}
