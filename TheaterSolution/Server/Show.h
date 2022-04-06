#pragma once

#include <time.h>

#define MAX 100

typedef struct Show
{
	char name[MAX];
	tm* datetime;
	int capacity;
	int available_seats;
} Show;