#include "Theater.h"

void readTheaters(LIST* theaters)
{
	char* buf;
	int* done = NULL, * err = NULL;
	FILE* fp;
	fp = fopen("shows.csv", "r");
	while (done == NULL || *done != 1)
	{
		int no_shows;
		Theater* theater;
		theater = malloc(sizeof(Theater));
		buf = fread_csv_line(fp, 1024, done, err);
		sscanf(buf, "%[^ ,],[^ ,],%d", theater->name, theater->location, &no_shows);
		while (--no_shows >= 0)
		{
			Show* show;
			show = malloc(sizeof(Show));
			char stime[100];
			buf = fread_csv_line(fp, 1024, done, err);
			sscanf(buf, "%[^ ,],%[^ ,],%d,%d", show->name, stime, &show->capacity, &show->available_seats);
			strptime(stime, "%c", show->datetime);
			InsertIni(theater->shows, show);
		}
		InsertIni(theaters, theater);
	}
	fclose(fp);
}

void writeTheaters(LIST theaters)
{
	FILE* fp;
	fp = fopen("theaters.csv", "w");
	LIST_NODE* tmp, * tmp2;
	tmp = theaters;
	while (tmp != NULL)
	{
		Theater* theater = DATA(tmp);
		int no_shows = ListSize(theater->shows);
		fprintf(fp, "\"%s\",\"%s\",%d\n", theater->name, theater->location, no_shows);
		tmp2 = theater->shows;
		while (tmp2 != NULL)
		{
			Show* show = DATA(tmp2);
			char stime[100];
			strftime(stime, sizeof(stime), "%c", show->datetime);
			fprintf(fp, "\"%s\",\"%s\",%d,%d\n", show->name, stime, show->capacity, show->available_seats);
			tmp2 = NEXT(tmp2);
		}
		tmp = NEXT(tmp);
	}
	fclose(fp);
}