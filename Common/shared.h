#pragma once
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "structures.h"
#include "../LoadBalancer/Queue.h"
#include "../LoadBalancer/WorkerList.h"
#define SafeCloseHandle(handle) if(handle) CloseHandle(handle);

enum EWorkers { L1 = 1, L2 = 2 };

CRITICAL_SECTION csOutput;
CRITICAL_SECTION csQ;
CRITICAL_SECTION csBusy, csFree;
QueueItem* headQ, * tailQ;
Worker* headBusy, * tailBusy;
Worker* headFree, * tailFree;

int freeWorkers;
WorkerData workerData;

void print(const char* format, ...) { //Easy print with critical section
	va_list args;
	va_start(args, format);
	EnterCriticalSection(&csOutput);
	printf("\n");
	vprintf(format, args);
	LeaveCriticalSection(&csOutput);
	va_end(args);
}

bool EnqueueSafe(char* data) {
	    bool res = false;
		EnterCriticalSection(&csQ);
		res = Enqueue(&headQ, &tailQ, data);
		LeaveCriticalSection(&csQ);
	

	if (res)
		print("Successfully added %s to queue.", data);
	else
		print("Cannot add to queue");
	return res;
}

bool DequeueSafe(char** data) {
	bool res = false;
		EnterCriticalSection(&csQ);
		res = Dequeue(&headQ, &tailQ, data);
		LeaveCriticalSection(&csQ);
	return res;
}

Worker* AddWorkerSafe(WorkerData data, EWorkers list) {
	Worker* worker = NULL;
	
	if (list == L1) {
		EnterCriticalSection(&csFree);
		worker = AddWorker(&headFree, &tailFree, data);
		freeWorkers++;
		LeaveCriticalSection(&csFree);
	}
	else
	{
		EnterCriticalSection(&csBusy);
		worker = AddWorker(&headBusy, &tailBusy, data);
		//freeWorkers--;
		LeaveCriticalSection(&csBusy);
	}
	print("Successfully added worker %d to queue %d",data.socket,list);
	return worker;
}

bool RemoveExactWorkerSafe(Worker* w) {
	bool done = false;
	int s = w->data.socket;
	EnterCriticalSection(&csBusy);
	done = RemoveExactWorker(&headBusy, &tailBusy,w);
	LeaveCriticalSection(&csBusy);
	print("Successfully removed worker %d from busyWorkers",s);
	return done;
}

Worker* RemoveWorkerSafe(EWorkers list) {
	Worker* worker = NULL;
	if (list == L1) {
		EnterCriticalSection(&csFree);
		worker = RemoveWorker(&headFree, &tailFree);
		freeWorkers--;
		LeaveCriticalSection(&csFree);
	}
	else
	{
		EnterCriticalSection(&csBusy);
		worker = RemoveWorker(&headBusy, &tailBusy);
		freeWorkers++;
		LeaveCriticalSection(&csBusy);
	}
	print("Successfully removed worker  to queue %d", list);
	return worker;
}

bool CloseWorkerSafe(SOCKET socket, EWorkers list) {
	bool ret = false;
	if (list == L1) {
		EnterCriticalSection(&csFree);
		ret = CloseWorker(&headFree, &tailFree, socket);
		freeWorkers--;
		LeaveCriticalSection(&csFree);
	}
	else
	{
		EnterCriticalSection(&csBusy);
		ret = CloseWorker(&headBusy, &tailBusy, socket);
		LeaveCriticalSection(&csBusy);
	}
	return ret;
}

void ShutdownWorker(SOCKET socket, EWorkers list) {
	if (list == L1) {
		EnterCriticalSection(&csFree);
		if (!ShutdownWorker(&headFree, &tailFree, socket))
			print("Couldn't shutdown worker");
		freeWorkers--;
		LeaveCriticalSection(&csFree);
	}
	else
	{
		EnterCriticalSection(&csBusy);
		if (!ShutdownWorker(&headBusy, &tailFree, socket))
			print("Couldn't shutdown worker");
		LeaveCriticalSection(&csBusy);
	}
}

Worker* FindWorkerForSocket(SOCKET socket, EWorkers list) {
	Worker* ret = NULL;
	if (list == L1) {
		EnterCriticalSection(&csFree);
		if (headFree == NULL)
			ret = NULL;
		else if (headFree->data.socket == socket)
			ret = headFree;
		else if (tailFree->data.socket == socket)
			ret = tailFree;
		else {
			Worker* temp = headFree->next;
			while (temp != NULL) {
				if (temp->data.socket == socket) {
					ret = temp;
					break;
				}

				temp = temp->next;
			}
		}
		LeaveCriticalSection(&csFree);
	}
	else
	{
		EnterCriticalSection(&csBusy);
		if (headBusy == NULL)
			ret = NULL;
		else if (headBusy->data.socket == socket)
			ret = headBusy;
		else if (headBusy->data.socket == socket)
			ret = tailBusy;
		else {
			Worker* temp = headBusy->next;
			while (temp != NULL) {
				if (temp->data.socket == socket) {
					ret = temp;
					break;
				}

				temp = temp->next;
			}
		}
		LeaveCriticalSection(&csBusy);
	}
	return ret;
}