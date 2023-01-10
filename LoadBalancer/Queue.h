#pragma once
#include "../Common/structures.h"

//Pops last item of queue (string in ret)
bool Dequeue(QueueItem** head, QueueItem** tail, char** ret) {
	if (*head == NULL)
		return false;

	*ret = (char*)malloc(BUFFER_SIZE);
	memcpy(*ret, (*head)->data, BUFFER_SIZE);
	QueueItem* temp = *head;
	if (*tail == *head) *tail = NULL;
	*head = (*head)->next;
	free(temp);
	return true;
}

//Pushes to beginning of queue
bool Enqueue(QueueItem** head, QueueItem** tail, char* data) {
	QueueItem* temp = (QueueItem*)malloc(sizeof(QueueItem));
	if (temp == NULL)
		return false;

	memcpy(temp->data, data, BUFFER_SIZE);
	temp->next = NULL;
	if (*tail == NULL) {
		*head = *tail = temp;
		return true;
	}

	(*tail)->next = temp;
	*tail = temp;
	return true;
}

//Clears queue
void ClearQueue(QueueItem** head) {
	if (*head != NULL) {
		ClearQueue(&((*head)->next));
		free(*head);
		*head = NULL;
	}
}
