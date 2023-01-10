#pragma once
#include "../Common/shared.h"
#include "../Common/structures.h"
#include "ClientHandler.h"

timeval timeVal = { 1,0 };
FD_SET readfds;
char pombuffer[BUFFER_SIZE];
int id;

DWORD WINAPI ReceiverHandler(LPVOID param) 
{
	while (true)
	{
		FD_ZERO(&readfds);
		char dataBuffer[BUFFER_SIZE];

		if (headFree != NULL) {


			Worker* pom = headFree;
			

			while (pom != NULL)
			{
				FD_SET(pom->data.socket, &readfds);
				pom = pom->next;
			}

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
				while (headFree != NULL)
				{
					// Check if new message is received from client on position "i"
					//treba da primi Zahtev je obradjen
					if (FD_ISSET(headFree->data.socket, &readfds))
					{
						int iResult = recv(headFree->data.socket, dataBuffer, BUFFER_SIZE, 0);
						if (iResult > 0)
						{
							dataBuffer[iResult] = 0;
							print("Response from Workeer: %s", dataBuffer);
							CloseWorkerSafe(headFree->data.socket, L2);
						}
						else if (iResult == 0)
						{
							// connection was closed gracefully
							printf("Connection with worker closed.\n");
							CloseWorkerSafe(headFree->data.socket, L2);

						}
						else
						{
							// there was an error during recv
							printf("recv failed with error: %d\n", WSAGetLastError());
							CloseWorkerSafe(headFree->data.socket, L2);
							Sleep(1000);

						}
					}
					if (headFree == NULL) {
						Sleep(5000);
						continue;
					}
					else
					{
						headFree = headFree->next;
					}
				}
			}
		}






		if (headBusy != NULL)
		{
			Worker* pom = headBusy;
		
			while (pom != NULL)
			{
				FD_SET(pom->data.socket, &readfds);
				pom = pom->next;
			}


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
				// Check if new message is received from connected clients
				while (headBusy != NULL)
				{
					// Check if new message is received from client on position "i"
					//treba da primi Zahtev je obradjen
					if (FD_ISSET(headBusy->data.socket, &readfds))
					{
						int iResult = recv(headBusy->data.socket, dataBuffer, BUFFER_SIZE, 0);
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
							//treba da nadje kijenta sa id i poalje mu poruku?
							for (int i = 0; i < NUM_OF_CLIENTS; i++)
							{
								if (clients[i].id == id)
								{
									int sResult = send(clients[i].socket, "Zahtev obradjen!", 17, 0);
									//izbaci iz busy i u free
									Worker* w = FindWorkerForSocket(headBusy->data.socket, L2);
									Worker pom = *w;
									RemoveExactWorkerSafe(w);
									AddWorkerSafe(pom.data, L1);
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
							//CloseWorkerSafe(headBusy->data.socket,L2)


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
							//closesocket(head->data.socket);
							CloseWorkerSafe(headBusy->data.socket, L2);
							Sleep(1000);

						}
					}

					if (headBusy == NULL) {
						Sleep(5000);
						continue;
					}
					else
					{
						headBusy = headBusy->next;
					}
					
				}
			}
		}
		else
		{
			print("Waiting!");
			Sleep(3000);
			//continue;
		}
	}
	return 0;

}