#include "Theaters.h"

std::map<std::string, Theater*> theaters;

void WriteTheaters()
{
	for (const auto& [name, theater] : theaters)
	{
		theater->Write();
	}
}

void ReadTheatersFromFile(const char* filename)
{
	io::CSVReader<6> reader(filename);
	reader.read_header(io::ignore_no_column,
		"theater", "location", "name", "datetime", "capacity", "available_seats");

	std::string theater_name, location, name, datetime;
	int capacity, available_seats;

	while (reader.read_row(theater_name, location, name,
		datetime, capacity, available_seats))
	{
		if (!theaters.contains(theater_name))
		{
			Theater* theater = new Theater;
			theater->name = theater_name;
			theater->location = location;
			theaters.insert(std::make_pair(theater_name, theater));
		}
		Show* show = new Show;
		show->name = name;
		std::istringstream ss{ datetime };
		ss >> std::get_time(&show->datetime, "%a %b %d %H:%M:%S %Y");
		show->capacity = capacity;
		show->available_seats = available_seats;
		theaters[theater_name]->shows.push_back(show);
	}
}

void WriteTheatersToFile(const char* filename)
{
	std::ofstream ofs{ filename };
	ofs << "theater,location,name,datetime,capacity,available_seats\n";
	for (const auto& [name, theater] : theaters)
	{
		theater->WriteFile(ofs);
	}
}