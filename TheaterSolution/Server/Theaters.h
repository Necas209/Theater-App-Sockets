#pragma once
#include <sstream>
#include <map>
#include "Theater.h"
#include "csv.h"

extern std::map<std::string, Theater*> theaters;

void WriteTheaters();
void ReadTheatersFromFile(const char* filename = "shows.csv");
void WriteTheatersToFile(const char* filename = "shows.csv");