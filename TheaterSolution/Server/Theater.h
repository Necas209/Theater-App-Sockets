#pragma once

#include "Show.h"
#include "List.h"
#include "CSV/csv.h"
#include "stringtotime.h"
#include "Theater.h"

#define MAX 100

typedef struct Theater
{
	char name[MAX];
	char location[MAX];
	LIST shows = NULL;
} Theater;

void readTheaters(LIST* theaters);
void writeTheaters(LIST theaters);