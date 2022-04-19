#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>

class Show
{
public:
	int id;
	std::string name;
	tm datetime;
	int capacity;
	int available_seats;

	Show(int id, std::string name, int capacity, int available_seats);
	virtual ~Show();
	void Write();
	void WriteFile(std::ofstream& ofs);
};