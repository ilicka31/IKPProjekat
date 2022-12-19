#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>

typedef struct
{
    int idWorker;
    SOCKET* socket;
} Worker;


static int id = 0;

Worker* create_new_worker(SOCKET* socket)
{
    Worker* novi = (Worker*)malloc(sizeof(Worker));
    if (novi == NULL)
        return (NULL);
    novi->idWorker = ++id;
    novi->socket = socket;
    return novi;
}

void delete_worker(Worker* c)
{
    if (c != NULL)
        free(c);
}
