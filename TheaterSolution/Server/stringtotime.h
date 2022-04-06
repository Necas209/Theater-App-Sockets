#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _MSC_VER
const char* strp_weekdays[] = { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" };
const char* strp_monthnames[] = { "january", "february", "march", "april", "may", "june", "july",
									"august", "september", "october", "november", "december" };

bool strp_atoi(const char** s, int* result, int low, int high, int offset);
char* strptime(const char* s, const char* format, struct tm* tm);
#endif