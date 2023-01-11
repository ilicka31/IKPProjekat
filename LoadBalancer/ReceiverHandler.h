#pragma once
#include "../Common/shared.h"
#include "../Common/structures.h"
#include "ClientHandler.h"

timeval timeVal = { 1,0 };
FD_SET readfds;
char pombuffer[BUFFER_SIZE];
int id;



DWORD WINAPI ReceiverFreeHandler(LPVOID param) {

	while (true)
	{
		char dataBuffer[BUFFER_SIZE];
		if (headFree != NULL) {

			FD_ZERO(&readfds);

			Worker* pom = headFree;


			while (pom != NULL)
			{
				FD_SET(pom->data.socket, &readfds);
				pom = pom->next;
			}
			pom = headFree;

			int selectResult = select(0, &readfds, NULL, NULL, &timeVal);

			if (selectResult == SOCKET_ERROR)
			{
				printf("Select failed with error: %d\n", WSAGetLastError());
				WSACleanup();
				return 1;
			}
			else if (selectResult == 0) // timeout expired
			{
				Sleep(1000);

			}
			else
			{
				
				if (FD_ISSET(headFree->data.socket, &readfds))
				{   //da slusa kad se odjavi
					int iResult = recv(headFree->data.socket, dataBuffer, BUFFER_SIZE, 0);
					if (iResult > 0)
					{
						dataBuffer[iResult] = 0;
						print("Response from Worker: %s", dataBuffer);
						CloseWorkerSafe(headFree->data.socket, L1);
					}
					else if (iResult == 0)
					{
						// connection was closed gracefully
						printf("Connection with worker closed.\n");
						CloseWorkerSafe(headFree->data.socket, L1);

					}
					else
					{
						if (WSAGetLastError() == WSAEWOULDBLOCK)
						{
							Sleep(100);
							continue;
						}
						// there was an error during recv
						printf("recv failed with error: %d\n", WSAGetLastError());
						CloseWorkerSafe(headFree->data.socket, L1);
						Sleep(1000);

					}
				}
				if (headFree == NULL) {
					Sleep(2000);
					continue;
				}
				pom = pom->next;
			}
		}
		else
		{
			print("Waiting FREE!");
			Sleep(2000);
			continue;
		}

	}

}
DWORD WINAPI ReceiverBusyHandler(LPVOID param) 
{
	while (true)
	{
		
		char dataBuffer[BUFFER_SIZE];

		if (headBusy != NULL)
		{
			FD_ZERO(&readfds);	
			Worker* pom = headBusy;
		
			while (pom != NULL)
			{
				FD_SET(pom->data.socket, &readfds);
				pom = pom->next;
			}
			pom = headBusy;

			int selectResult = select(0, &readfds, NULL, NULL, &timeVal);

			if (selectResult == SOCKET_ERROR)
			{
				printf("Select failed with error: %d\n", WSAGetLastError());
				WSACleanup();
				return 1;
			}
			else if (selectResult == 0)
			{
				Sleep(1000);
			}
			else
			{
					if (FD_ISSET(pom->data.socket, &readfds))
					{
						int iResult = recv(pom->data.socket, dataBuffer, BUFFER_SIZE, 0);
						if (iResult > 0)
						{
							dataBuffer[iResult] = 0;
							print("Response from Worker: %s", dataBuffer);
							char* token = strtok(dataBuffer, "$");
							bool prvi = false;
							while (token != NULL) {
								if (!prvi) {
									strcpy(pombuffer, token); //printing each token
									prvi = true;
								}
								else
								{
									id = atoi(token);
								}
								token = strtok(NULL, "$");
							}
							bool flag = false;
							for (int i = 0; i < NUM_OF_CLIENTS; i++)
							{
								if (clients[i].id == id)
								{
									int sResult = send(clients[i].socket, "Zahtev obradjen!", 17, 0);
									//izbaci iz busy i u free
									Worker* w = FindWorkerForSocket(pom->data.socket, L2);
									SafeMove(w, L2);
									
									flag = true;
									break;
								}
								
							}
							if (!flag)
							{
								print("Klijent se diskonektovao!");
							}

						}
						else if (iResult == 0)
						{
							// connection was closed gracefully
							printf("Connection with worker closed.\n");
							CloseWorkerSafe(headBusy->data.socket, L2);
						}
						else
						{
							if (WSAGetLastError() == WSAEWOULDBLOCK)
							{
								Sleep(100);
								continue;
							}
							// there was an error during recv
							printf("recv failed with error: %d\n", WSAGetLastError());
							CloseWorkerSafe(headBusy->data.socket, L2);
							Sleep(1000);
						}
					}

					if (headBusy == NULL) {
						Sleep(2000);
						continue;
					}	
					pom = pom->next;	
			}
		}
		else
		{
			print("Waiting BUSY!");
			Sleep(2000);
			continue;
		}
	}
	return 0;

}