#pragma once
#include <stdio.h>
#include <winsock.h>
#define BUFFER_SIZE 512

typedef struct QueueItem {
    char data[BUFFER_SIZE] = "";
    struct QueueItem* next = NULL;
} QueueItem;

typedef struct WorkerData {
	HANDLE handle = NULL;
	SOCKET socket = INVALID_SOCKET;
} WorkerData;

typedef struct Worker {
	WorkerData data;
	bool shuttingDown = false;
	struct Worker* next = NULL;
} Worker;

typedef struct Client {
	int id;
	HANDLE handle = NULL;
	SOCKET socket = INVALID_SOCKET;
	bool isUsed = false;
} Client;

typedef struct ClientPacket {
	int count;
	SOCKET socket = INVALID_SOCKET;
}ClientPacket;