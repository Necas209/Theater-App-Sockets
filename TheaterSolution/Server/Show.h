#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>

class Show
{
public:
	std::string name;
	tm datetime;
	int capacity;
	int available_seats;

	Show(std::string name, int capacity, int available_seats);
	virtual ~Show();
	void Write();
	void WriteFile(std::ofstream& ofs);
};