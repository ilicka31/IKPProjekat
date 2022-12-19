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


#define SERVER_IP_ADDRESS "127.0.0.1"
#define BUFFER_SIZE 512
#define SERVER_PORT 27017

int main(int argc, char** argv)
{
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // message to send
    char dataBuffer[BUFFER_SIZE];

    // Validate the parameters
    /*  if (argc != 2)
      {
          printf("usage: %s server-name\n", argv[0]);
          return 1;
      }
    */
    if (InitializeWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

    // create a socket
    connectSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // create and initialize address structure
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
    serverAddress.sin_port = htons(SERVER_PORT);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }
    do {
        printf("Enter message to send: ");
        gets_s(dataBuffer);
        // Send an prepared message with null terminator included
        iResult = send(connectSocket, dataBuffer, (int)strlen(dataBuffer), 0);

        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
    } while (strcmp(dataBuffer, "KRAJ"));

    // Shutdown the connection since we're done
    iResult = shutdown(connectSocket, SD_BOTH);

    // Check if connection is succesfully shut down.
    if (iResult == SOCKET_ERROR)
    {
        printf("Shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    // For demonstration purpose
    printf("\nPress any key to exit: ");
    _getch();


    // Close connected socket
    closesocket(connectSocket);

    // Deinitialize WSA library
    WSACleanup();
    return 0;
}
