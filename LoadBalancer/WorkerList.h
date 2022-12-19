#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Worker.h"

typedef struct WorkersList
{
    Worker* w;
    struct WorkerList* next;
} WorkersList;

static int id = 0;

void init_list(WorkersList** head)
{
    *head = NULL;
}

void add_to_list(Worker* w, WorkersList** head)
{
    if (*head == NULL)
    {
        WorkersList* temp = (WorkersList*)malloc(sizeof(WorkersList));
        (temp)->w = w;
        (temp)->next = NULL;
        (*head) = temp;
    }
    else
    {
        WorkersList* temp = (WorkersList*)malloc(sizeof(WorkersList));
        (temp)->w = w;
        (temp)->next = head;
        (*head) = temp;
    }
}
/*
void add_to_list(Worker *w, WorkersList **head){
    WorkersList *temp = (WorkersList*)malloc(sizeof(WorkersList));
    temp->next = NULL;
    temp->w = w;

    WorkersList *pomocna = NULL;

    if(*head == NULL){
        (*head)=temp;
    }
    else{
        while(head->next != NULL){
            pomocna = head->next;
        }
        pomocna->next=temp;
    }
}
*/
void remove_from_list(int id);



void delete_list(WorkersList** head)
{
    WorkersList* temp = *head;
    while (temp != NULL)
    {
        *head = temp->next;
        free(temp);
        temp = *head;
    }
}
