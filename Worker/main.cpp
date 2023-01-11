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
#pragma warning(disable : 4703)
#pragma pack(1)
#include "../Common/communication.h"


#define BUFFER_SIZE 512


int main(int argc, char** argv)
{
    SOCKET socket = ConnectSocket(4000);
    if (socket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }



    char buffer[BUFFER_SIZE];
    char pombuffer[BUFFER_SIZE];
    char* id;
    FD_SET read, write;
    timeval time;
    time.tv_sec = 1;
    time.tv_usec = 0;
    char message[] = "Zahtev je obradjen!$";
    char odjava[] = "Odjavljujem se!";
    char *sendMessage;
    printf("Press q to exit...\n");

    while (true) { 
        FD_ZERO(&write);
        FD_SET(socket, &write);

        if (_kbhit()) {
            if (_getch() == 'q') {
                int sel = select(0, NULL, &write, NULL, &time);
                if (sel == SOCKET_ERROR)
                {
                    printf("Select error: %d\n", WSAGetLastError());
                }
                else if (sel == 0)
                {
                    Sleep(2000);
                    continue;
                }
                else if (sel > 0) {
                    if (FD_ISSET(socket, &write)) {
                        char* pom = odjava;
                        pom[strlen(odjava)] = 0;
                        int iResult = send(socket, pom, strlen(pom), 0);
                        if (iResult == SOCKET_ERROR)
                            printf("Error send: %d\n", WSAGetLastError());
                        break;
                    }
                }
            }
        }

        FD_ZERO(&read);
        FD_SET(socket, &read);

       

        int sResult = select(0, &read, &write, NULL, &time);
        if (sResult == SOCKET_ERROR)
        {
            printf("Select error: %d\n", WSAGetLastError());
            break;
        }
        else if (sResult == 0)
        {
            Sleep(2000);
            continue;
        }
        else if (sResult > 0) {
            if (FD_ISSET(socket, &read)) {
                int iResult = recv(socket, buffer, BUFFER_SIZE, 0);
                if (iResult > 0) {
                    buffer[iResult] = 0;
                    printf("\nPristigla poruka: %s.", buffer);
                    char* token = strtok(buffer, "$");
                    bool prvi = false;
                    while (token != NULL) {
                        if (!prvi) {
                            strcpy(pombuffer, token);
                            prvi = true;
                        }
                        else
                        {
                            id =token;
                        }
                        token = strtok(NULL, "$");
                    }
                    Sleep(5000);
                    strcpy(pombuffer, message);
                    strcat(pombuffer, id);
                    pombuffer[strlen(buffer)] = 0;

                   // if (FD_ISSET(socket, &write)) {
                        printf("\nSaljem nazad LoadBalanceru: %s", pombuffer);
                        int iResult = send(socket, pombuffer, strlen(pombuffer), 0);
                        if (iResult == SOCKET_ERROR)
                            printf("Error send: %d\n", WSAGetLastError());
                        
                  //  }
                    
                }
            }
        }
    }
    printf("\nOdjavljen!");
    shutdown(socket, SD_BOTH);
    closesocket(socket);
    WSACleanup();
    _getch();
    return 0;
}
