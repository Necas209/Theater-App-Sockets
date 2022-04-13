#pragma once
#include <iomanip>
#include <sstream>
#include <map>
#include "Theater.h"
#include "csv.h"

extern std::map<std::string, Theater*> theaters;

void WriteTheaters();
void ReadTheatersFromFile();
void WriteTheatersToFile();