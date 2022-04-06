#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _LIST_NODE
{
    void *data;
    struct _LIST_NODE *next;
} LIST_NODE;
typedef LIST_NODE *LIST;

typedef enum _LIST_LOCATION
{
    LIST_START,
    LIST_END
} LIST_LOCATION;

typedef enum _STATUS
{
    OK,
    ERROR
} STATUS;

#define DATA(node)((node)->data)
#define NEXT(node)((node)->next)
#define EMPTY NULL
#define NO_LINK NULL

void initList(LIST *list);
bool emptyList(LIST list);
LIST_NODE *NewNode(void *data);
STATUS InsertIni(LIST *list, void *data);
STATUS InsertPos(LIST* list, void* data, int pos);
STATUS InsertEnd(LIST *list, void *data);
void RemoveIni(LIST* list);
void RemoveEnd(LIST* list);
int ListSize(LIST list);
void ShowValues(LIST list);