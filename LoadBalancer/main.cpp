#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include <stdbool.h>
#include <pthread.h>
#include <time.h>


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma pack(1)

//#include "hashmap.h"
#include "../Common/communication.h"
#include "../Common/shared.h"
#include "Queue.h"
#include "ClientHandler.h"
#include "WorkerList.h"
#include "WorkerHandler.h"
#include "DispatcherHandler.h"
#include "ReceiverHandler.h"

#pragma comment(lib, "Ws2_32.lib")
#define SERVER_PORT 27016
#define MAX_CLIENTS 10
	

int main(void)
{
	SOCKET listenClients = SetListenSocket("5059"), listenWorkers = SetListenSocket("4000");
	if (listenClients == INVALID_SOCKET || listenWorkers == INVALID_SOCKET) {
		WSACleanup();
		return 1;
	}

	freeWorkers = 0;
	headBusy = NULL, headFree = NULL;
	tailBusy = NULL, tailFree = NULL;
	headQ = NULL, tailQ=NULL;
    HANDLE threadDispatcher = CreateThread(NULL, 0, &DispatcherHandler, NULL, 0, NULL);
	HANDLE threadReceiverFree = CreateThread(NULL, 0, &ReceiverFreeHandler, NULL, 0, NULL);
	HANDLE threadReceiverBusy = CreateThread(NULL, 0, &ReceiverBusyHandler, NULL, 0, NULL);
	InitializeCriticalSection(&csOutput);
	InitializeCriticalSection(&csQ);
	InitializeCriticalSection(&csBusy);
	InitializeCriticalSection(&csFree);
	InitializeClients();

	print("Press q to exit...\n");
	FD_SET set;
	timeval time = { 5, 0 };

	SOCKADDR_IN address;
	int addrlen = sizeof(address);

	static int clientCount = 0;

	while (true) {
		FD_ZERO(&set);
		FD_SET(listenClients, &set);
		FD_SET(listenWorkers, &set);

		if (_kbhit()) {
			if (_getch() == 'q') {
				print("Closing...");
				break;
			}
		}

		int result = select(0, &set, NULL, NULL, &time);
		if (result == SOCKET_ERROR) {
			print("select error: %d", WSAGetLastError());
			break;
		}
		else if (result == 0) {
			Sleep(1000);
			continue;
		}
		else if (result > 0) {
			if (FD_ISSET(listenClients, &set)) {
				bool connected = false;
				for (int i = 0; i < MAX_CLIENTS; i++) {
					if (!clients[i].isUsed) {
						SOCKET socket = accept(listenClients, (sockaddr*)&address, &addrlen);
						if (socket == INVALID_SOCKET)
							break;
						++clientCount;
						ClientPacket cp = { clientCount, socket };
						HANDLE handle = CreateThread(NULL, 0, &ClientHandler, &cp, 0, NULL);
						if (handle == NULL) {
							print("Client couldn't make thread");
							break;
						}
						clients[i] = {clientCount,handle, socket, true };
						connected = true;
						break;
					}
				}

				if (connected) continue;

				print("Too much clients, can't connect");
				SOCKET socket = accept(listenWorkers, (sockaddr*)&address, &addrlen);
				if (socket == INVALID_SOCKET)
					break;

				shutdown(socket, SD_BOTH);
				closesocket(socket);
				continue;
			}
			else if (FD_ISSET(listenWorkers, &set)) {
				if (freeWorkers >= 10) {
					print("Too much workers, can't connect");
					SOCKET socket = accept(listenWorkers, (sockaddr*)&address, &addrlen);
					if (socket == INVALID_SOCKET)
						break;

					shutdown(socket, SD_BOTH);
					closesocket(socket);
					continue;
				}

				SOCKET socket = accept(listenWorkers, (sockaddr*)&address, &addrlen);
				if (socket == INVALID_SOCKET) {
					print("Accept error: %d", WSAGetLastError());
					continue;
				}
				
				newWorker->data.socket = socket;
				newWorker->shuttingDown = false;
				newWorker->next = NULL;
				newWorker->data.handle = CreateThread(NULL, 0, &WorkerRegistry, newWorker, 0, NULL);

			}
		}
	}

	free(newWorker);
	tailQ = NULL;
	tailBusy =NULL;
	tailFree = NULL;
	ClearQueue(&headQ);
	CloseAllWorkers(&headBusy);
	CloseAllWorkers(&headFree);
	CloseAllClients();

	closesocket(listenClients);
	closesocket(listenWorkers);
	WSACleanup();

	DeleteCriticalSection(&csOutput);
	DeleteCriticalSection(&csQ);
	DeleteCriticalSection(&csBusy);
	DeleteCriticalSection(&csFree);
	SafeCloseHandle(threadDispatcher);
	SafeCloseHandle(threadReceiverFree);
	SafeCloseHandle(threadReceiverBusy);
	_getch();
	return 0;
}

