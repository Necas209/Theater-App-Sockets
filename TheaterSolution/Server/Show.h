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

	Show();
	virtual ~Show();
	void Write();
	void WriteFile(std::ofstream& ofs);
};