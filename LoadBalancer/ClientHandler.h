#pragma once

#include "../Common/shared.h"
#include "../Common/structures.h"

#define NUM_OF_CLIENTS 10
Client clients[NUM_OF_CLIENTS];

void InitializeClients() {
	for (int i = 0; i < NUM_OF_CLIENTS; i++) {
		clients[i].isUsed = false;
	}
}

void CloseClient(SOCKET socket) {
	for (int i = 0; i < NUM_OF_CLIENTS; i++) {
		if (clients[i].socket == socket) {
			clients[i].isUsed = false;
			closesocket(socket);
			SafeCloseHandle(clients[i].handle);
			return;
		}
	}
}



void CloseAllClients() {
	for (int i = 0; i < NUM_OF_CLIENTS; i++) {
		if (clients[i].isUsed) {
			clients[i].isUsed = false;
			closesocket(clients[i].socket);
			SafeCloseHandle(clients[i].handle);
		}
	}
}

DWORD WINAPI ClientHandler(LPVOID param) {
	ClientPacket cp = *(ClientPacket*)param;
	int id = cp.count;
	SOCKET socket = cp.socket;
	print("Client %d connected, id client: %d", socket, id);
	FD_SET set;
	char buffer[BUFFER_SIZE];
	timeval time = { 1, 0 };
	char pombuffer[BUFFER_SIZE]="";
	

	while (true)
	{
		FD_ZERO(&set);
		FD_SET(socket, &set);

		int result = select(0, &set, NULL, NULL, &time);
		if (result == SOCKET_ERROR) {
			CloseClient(socket);
			print("Error: %d", WSAGetLastError());
			break;
		}
		else if (result > 0) {
			int iResult = recv(socket, buffer, BUFFER_SIZE, 0);
			if (iResult > 0) {
				if (iResult < BUFFER_SIZE) {
					char c[5];
					strcpy(pombuffer, buffer);
					strcat(pombuffer, "$");
					itoa(id,c,10);
					strcat(pombuffer, c);
					pombuffer[strlen(pombuffer)] = 0;
				}
				if (!EnqueueSafe(pombuffer))
					continue;
			}
			else if (iResult == 0) {
				print("Client %d disconnected", socket);
				CloseClient(socket);
				break;
			}
			else {
				print("Error client %d: %d", socket, WSAGetLastError());
				CloseClient(socket);
				break;
			}
		}
	}
	return 0;
}