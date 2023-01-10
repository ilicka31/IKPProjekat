#pragma once

#include "../Common/shared.h"
#include "../Common/structures.h"
#include "ReceiverHandler.h"
HANDLE senderThread;
HANDLE receiverThread;

DWORD WINAPI SenderHandler(LPVOID param)
{
	char* data = (char*)param;

	Worker* w = RemoveWorkerSafe(L1);

	FD_SET set;
	timeval time = { 1, 0 };

	FD_ZERO(&set);
	FD_SET(w->data.socket, &set);


	int sResult = select(0, NULL, &set, NULL, &time);
	if (sResult == SOCKET_ERROR) {
		print("Error select: %d", WSAGetLastError());
	}
	if (FD_ISSET(w->data.socket, &set)) {
		int iResult = send(w->data.socket, data, strlen(data), 0);
		if (iResult == SOCKET_ERROR)
			return false;
		AddWorkerSafe(w->data, L2);
	}
}


DWORD WINAPI DispatcherHandler(LPVOID param) {
	while (true)
	{
		char* data;
		if (!freeWorkers)
		{
			Sleep(1000);
			continue;
		}
		else 
		{
			if (DequeueSafe(&data))
			{
				//senderThread = CreateThread(NULL, 0, &SenderHandler, &data, 0, NULL);
				Worker* w = RemoveWorkerSafe(L1);

				FD_SET set;
				timeval time = { 1, 0 };

				FD_ZERO(&set);
				FD_SET(w->data.socket, &set);


				int sResult = select(0, NULL, &set, NULL, &time);
				if (sResult == SOCKET_ERROR) {
					print("Error select: %d", WSAGetLastError());
				}
				if (FD_ISSET(w->data.socket, &set)) {
					int iResult = send(w->data.socket, data, strlen(data), 0);
					if (iResult == SOCKET_ERROR)
						return false;
					AddWorkerSafe(w->data, L2);
				}
			}
			else
			{
				print("Waiting!");
				Sleep(3000);
				//continue;
			}

		
		}
	}
	return 0;
}