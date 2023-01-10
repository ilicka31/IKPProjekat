#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SafeCloseHandle(handle) if(handle) CloseHandle(handle);
#include "../Common/structures.h"
#define MAX_WORKERS 10

Worker* newWorker = (Worker*)malloc(sizeof(Worker));
//Adds new worker to list
Worker* AddWorker(Worker** head, Worker** tail, WorkerData data) {
	Worker* temp = (Worker*)malloc(sizeof(Worker));
	
	temp->shuttingDown = false;
	temp->data.handle = data.handle;
	temp->data.socket = data.socket;

	temp->next = NULL;
	if (*tail == NULL) {
		*head = *tail = temp;
		return temp;
	}
	else {
		// List is not empty, update tail to point to the new node
		(*tail)->next = temp;
		*tail = temp;
	}
	return temp;
}

Worker* RemoveWorker(Worker** head, Worker** tail) {
	Worker* temp = (Worker*)malloc(sizeof(Worker));
	if (temp == NULL)
		return NULL;

	if (head == NULL)
		return NULL;

	if (tail ==NULL) {
		return NULL;
	}
	//temp = *tail;
	temp->data.handle = (*tail)->data.handle;
	temp->data.socket = (*tail)->data.socket;
	temp->next = (*tail)->next;
	temp->shuttingDown = (*tail)->shuttingDown;

	if (*head == *tail) {
		free(*head);
		*head = *tail = NULL;
	}
	else {
		Worker* newTail = *head;
		while (newTail->next != *tail) {
			newTail = newTail->next;
		}
		free(tail);
		*tail = newTail;
		(*tail)->next = NULL;
	}
	return temp;
}




bool RemoveExactWorker(Worker** head, Worker** tail, Worker* w) {
	Worker* temp = *head;
	Worker* prev = NULL;
	if (temp != NULL && temp->data.socket == w->data.socket)
	{
		*head = temp->next;
		if (*tail == temp) {
			*tail = NULL;
		}
		free(temp);
		return true;
	}
	while (temp != NULL && temp->data.socket != w->data.socket)
	{
		prev = temp;
		temp = temp->next;
	}
	if (temp == NULL) {
		return false;
	}
	if (prev != NULL) {
		prev->next = temp->next;
	}
	if (*tail == temp) {
		*tail = prev;
	}
	free(temp);
	return true;
}


//Moves first to last item in list
void FirstToLastWorker(Worker** head, Worker** tail) {
	if (*head == *tail)
		return;

	Worker* temp = *head;
	*head = (*head)->next;
	temp->next = NULL;
	(*tail)->next = temp;
	*tail = temp;
}

//Shutting down worker properly
bool ShutdownWorker(Worker** head, Worker** tail, SOCKET socket) {
	if ((*head)->data.socket == socket) {
		*head = (*head)->next;
		return true;
	}

	Worker* temp1 = *head, * temp2 = (*head)->next;
	while (temp2 != NULL) {
		if (temp2->data.socket == socket) {
			if (temp2 == *tail)
				*tail = temp1;

			temp1->next = temp2->next;
			return true;
		}

		temp1 = temp2;
		temp2 = temp2->next;
	}
	return false;
}

//Closes Worker for given socket
bool CloseWorker(Worker** head, Worker** tail, SOCKET socket) {
	if (*head == NULL)
		return false;

	if ((*head)->data.socket == socket) {
		Worker* temp = *head;
		*head = (*head)->next;
		closesocket(temp->data.socket);
		SafeCloseHandle(temp->data.handle);
		free(temp);
		return true;
	}

	Worker* temp1 = *head, * temp2 = (*head)->next;
	while (temp2 != NULL) {
		if (temp2->data.socket == socket) {
			if (temp2 == *tail)
				*tail = temp1;

			temp1->next = temp2->next;
			closesocket(temp2->data.socket);
			SafeCloseHandle(temp2->data.handle);
			free(temp2);
			return true;
		}

		temp1 = temp2;
		temp2 = temp2->next;
	}
	return false;
}

//Clears list
void CloseAllWorkers(Worker** head) {
	if (*head != NULL) {
		CloseAllWorkers(&((*head)->next));
		SafeCloseHandle((*head)->data.handle);
		closesocket((*head)->data.socket);
		free(*head);
		*head = NULL;
	}
}
