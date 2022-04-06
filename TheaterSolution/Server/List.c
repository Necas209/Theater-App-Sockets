#include "List.h"
#include "Theater.h"

/****************************************************************
* Funcao: Inicializa a lista 
* 
* Parametros: list - apontador para lista 
*                    (duplo apontador para o primeiro nó)              
* Saida:      void 
***************************************************************/ 
void initList(LIST *list) 
{ 
    *list = NULL; 
} 
/**************************************************************** 
* Funcao: verifica se a lista é vazia 
* 
* Parametros: list - apontador para lista 
* Saida:      TRUE se a lista for vazia, ERROR caso contrário 
***************************************************************/
bool emptyList(LIST list)
{
    return list == NULL;
} 
/**************************************************************** 
* Funcao: Cria um nó 
* 
* Parametros: data - apontador generico para os dados a inserir no nó criado 
* Saida:      apontador para o nó criado ou NULL em caso de erro 
***************************************************************/
LIST_NODE *NewNode(void *data)
{
    LIST_NODE *new_node;
    if ((new_node = malloc(sizeof(LIST_NODE))) != NULL)
    {
        DATA(new_node) = data;
        NEXT(new_node) = NULL;
    }
    return new_node;
}
/**************************************************************** 
* Funcao: Insere um nó no inicio da lista 
* 
* Parametros: list - apontador para lista 
*                    (duplo apontador para o primeiro nó) 
*             data - apontador generico para os dados a inserir no nó criado 
* Saida:      OK se o nó foi inserido na LISTA e ERROR caso contrário 
***************************************************************/ 
STATUS InsertIni(LIST *list, void *data)
{
    LIST_NODE *new_node;
    if ((new_node = NewNode(data)) != NULL)
    {
        NEXT(new_node) = *list;
        *list = new_node;
        return OK;
    }
    return ERROR;
}
/****************************************************************
* Funcao: Insere um nó numa determinada posição da lista
*
* Parametros: list - apontador para lista
*                    (duplo apontador para o primeiro nó)
*             data - apontador generico para os dados a inserir no nó criado
*             pos - posição da lista onde inserir o nó (0 a n - 1)
* Saida:      OK se o nó foi inserido na LISTA e ERROR caso contrário
***************************************************************/
STATUS InsertPos(LIST* list, void* data, int pos)
{
    LIST_NODE* new_node = NULL, * temp = NULL;
    int n = ListSize(*list);
    if (pos <= n)
    {
        if (pos == 0)
            return InsertIni(list, data);
        if (pos == n)
            return InsertEnd(list, data);
        if ((new_node = NewNode(data)) != NULL)
        {
            int i = 0;
            temp = list;
            while (i < (pos - 1))
            {
                temp = NEXT(temp);
                ++i;
            }
            NEXT(new_node) = NEXT(temp);
            NEXT(temp) = new_node;
            return OK;
        }
    }
    return ERROR;
}
/**************************************************************** 
* Funcao: Insere um nó no fim da lista 
* 
* Parametros: list - apontador para lista 
*                    (duplo apontador para o primeiro nó) 
*             data - apontador generico para os dados a inserir no nó criado 
* Saida:      OK se o nó foi inserido na lista e ERROR caso contrário 
***************************************************************/
STATUS InsertEnd(LIST *list, void *data)
{
    LIST_NODE *new_node, *temp;
    if ((new_node = NewNode(data)) != NULL)
    {
        if (*list == NULL)
            *list = new_node;
        else
        {
            temp = *list;
            while (NEXT(temp) != NULL)
                temp = NEXT(temp);
            NEXT(temp) = new_node;
        }
        return OK;
    }
    return ERROR;
}
/***************************************************************
* Funcao: Elimina um nó no início da lista
*
* Parametros: list - apontador para lista
*                    (duplo apontador para o primeiro nó)
***************************************************************/
void RemoveIni(LIST* list)
{
    LIST_NODE* next;
    if (!emptyList(*list))
    {
        next = NEXT(*list);
        free(DATA(*list));
        free(*list);
        *list = next;
    }
}
/***************************************************************
* Funcao: Elimina um nó no fim da lista
*
* Parametros: list - apontador para lista
*                    (duplo apontador para o primeiro nó)
***************************************************************/
void RemoveEnd(LIST* list)
{
    if (!emptyList(*list))
    {
        if (NEXT(*list) == NULL)
        {
            RemoveIni(list);
        }
        else
        {
            LIST_NODE* tmp = *list;
            while (NEXT(NEXT(tmp)) != NULL)
            {
                tmp = NEXT(tmp);
            }
            free(DATA(NEXT(tmp)));
            free(NEXT(tmp));
            NEXT(tmp) = NULL;
        }
    }
}
/***************************************************************************
* Funcao: calcula quantos elementos contem a lista 
* 
* Parametros: list - apontador para uma lista 
* Saida:      numero de elementos da lista 
***************************************************************************/ 
int ListSize(LIST list)
{
    int count = 0;
    while (list != NULL)
    {
        count++;
        list = NEXT(list);
    }
    return count;
} 
/**************************************************************** 
* Funcao: Escreve no ecra o conteudo da lista 
* 
* Parametros: list - apontador para o primeiro nó 
* Saida:      void 
***************************************************************/
void ShowValues(LIST list)
{
    if (emptyList(list))
    {
        printf("\nLista vazia.\n");
        return;
    }
    printf("LISTA ->");
    while (list != NULL)
    {
        Theater* theater = DATA(list);
        printf(" %s; %s; %d shows; ->", theater->name, theater->location, ListSize(theater->shows));
        list = NEXT(list);
    }
    printf(" FIM");
}