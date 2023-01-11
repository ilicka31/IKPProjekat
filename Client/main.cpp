#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include <stdbool.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma warning(disable : 4996)
#pragma pack(1)
#include "../Common/communication.h"




int main(int argc, char** argv)
{
	
	SOCKET socket = ConnectSocket(5059);
	if (socket == INVALID_SOCKET) {
		printf("Connection doesnt work %d", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return 1;
	}
	char buffer[BUFFER_SIZE];

	FD_SET setSend, setRcv;
	timeval time;
	time.tv_sec = 1;
	time.tv_usec = 0;
	const char* mess = "Klijent salje pozdrave!";
	bool flag = false;
	printf("Press q to exit...\n");
	Sleep(1000);

	while (true) {
		if (_kbhit())
			if (_getch() == 'q')
				break;


		FD_ZERO(&setSend);
		FD_SET(socket, &setSend);

		FD_ZERO(&setRcv);
		FD_SET(socket, &setRcv);

		int sResult = select(0, &setRcv, &setSend, NULL, &time);

		if (sResult == SOCKET_ERROR) {
			printf("ERROR select: %d\n", WSAGetLastError());
			shutdown(socket, SD_BOTH);
			closesocket(socket);
			WSACleanup();
			return 1;
		}
		else if (sResult == 0) {
			continue;
		}
		else if (sResult > 0) {
			if (FD_ISSET(socket, &setSend)) {

				if (!flag) {
					for (int i = 0; i < strlen(mess); i++)
					{
						buffer[i] = mess[i];
					}
					buffer[strlen(mess) - 1] = 0;
					printf("Klijent salje poruku: %s", buffer);
					int iResult = send(socket, buffer, strlen(mess), 0);
					if (iResult == SOCKET_ERROR) {
						printf("Error: %d\n", WSAGetLastError());
						shutdown(socket, SD_BOTH);
						closesocket(socket);
						WSACleanup();
						return 1;
					}
					flag = true;
				}
			}

			


			if (FD_ISSET(socket, &setRcv)) {

				int iResult = recv(socket, buffer, BUFFER_SIZE, 0);
				if (iResult > 0)
				{
					buffer[iResult] = 0;
					printf("\nMessage received from server: %s\n", buffer);
					break;
				}
				else if (iResult == 0)
				{
					Sleep(1000);
					continue;
				}
				else
				{
					// there was an error during recv
					printf("recv failed with error: %d\n", WSAGetLastError());
					closesocket(socket);
				}

			}
		}
		Sleep(5000);
		
	}
	printf("Safely returned");
	shutdown(socket, SD_BOTH);
	closesocket(socket);
	WSACleanup();
	_getch();
	return 0;
}
