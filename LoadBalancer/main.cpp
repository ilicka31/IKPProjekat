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

//#include "hashmap.h"
#include "../Common/communication.h"

#pragma comment(lib, "Ws2_32.lib")
#define BUFFER_SIZE 512
#define SERVER_PORT 27016
#define MAX_CLIENTS 50

int main(void)
{
    // Socket za prihvatanje novih klijenata
    SOCKET listenSocket = INVALID_SOCKET;
    // Socket za komunikaciju sa klijentom
    //SOCKET acceptedSocket = INVALID_SOCKET;


     //Sockets used for communication with client
	SOCKET clientSockets[MAX_CLIENTS];
	short lastIndex = 0;

    int iResult;
    // Buffer used for storing incoming data
    char dataBuffer[BUFFER_SIZE];

    if (InitializeWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

    // Prepare address information structures
    // Initialize serverAddress structure used by bind
    struct sockaddr_in serverAddress;
    memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;				// IPv4 address family
    serverAddress.sin_addr.s_addr = INADDR_ANY;		// Use all available addresses
    serverAddress.sin_port = htons(SERVER_PORT);	// Use specific port

    //lista klijentskih socketa setovana na 0
    //memset(clientSockets, 0, MAX_CLIENTS * sizeof(SOCKET));

    // Create a SOCKET for connecting to server
    listenSocket = socket(AF_INET,      // IPv4 address family
        SOCK_STREAM,  // Stream socket
        IPPROTO_TCP); // TCP protocol

// Check if socket is successfully created
    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket - bind port number and local address to socket
    iResult = bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Check if socket is successfully binded to address and port from sockaddr_in structure
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    //da se ne konektuju automatski
    bool bOptVal = true;
    int bOptLen = sizeof(bool);
    iResult = setsockopt(listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (char*)&bOptVal, bOptLen);
    if (iResult == SOCKET_ERROR) {
        printf("setsockopt for SO_CONDITIONAL_ACCEPT failed with error: %u\n", WSAGetLastError());
    }


    unsigned long  mode = 1;
    if (ioctlsocket(listenSocket, FIONBIO, &mode) != 0)
        printf("ioctlsocket failed with error.");

    // Set listenSocket in listening mode
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server socket is set to listening mode. Waiting for new connection requests.\n");

    // set of socket descriptors
    fd_set readfds;

    // timeout for select function
    timeval timeVal;
    timeVal.tv_sec = 1;
    timeVal.tv_usec = 0;

    while (true)
    {
        // initialize socket set
        FD_ZERO(&readfds);

        // add server's socket and clients' sockets to set
        if (lastIndex != MAX_CLIENTS)
        {
            FD_SET(listenSocket, &readfds);
        }

        for (int i = 0; i < lastIndex; i++)
        {
            FD_SET(clientSockets[i], &readfds);
        }

        // wait for events on set
        int selectResult = select(0, &readfds, NULL, NULL, &timeVal);

        if (selectResult == SOCKET_ERROR)
        {
            printf("Select failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        else if (selectResult == 0) // timeout expired
        {
            if (_kbhit()) //check if some key is pressed
            {
                _getch();
                printf("Istekao timeout\n");
            }
            continue;
        }
        else if (FD_ISSET(listenSocket, &readfds))
        {
            // Struct for information about connected client
            struct sockaddr_in clientAddr;

            int clientAddrSize = sizeof(struct sockaddr_in);

            // Accept new connections from clients
            // New connection request is received. Add new socket in array on first free position.
            clientSockets[lastIndex] = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);


            // Check if accepted socket is valid
            if (clientSockets[lastIndex] == INVALID_SOCKET)
            {
                if (WSAGetLastError() == WSAECONNRESET)
                {
                    printf("accept failed, because timeout for client request has expired.\n");
                }
                else
                {
                    printf("accept failed with error: %d\n", WSAGetLastError());
                }
            }
            else
            {
                if (ioctlsocket(clientSockets[lastIndex], FIONBIO, &mode) != 0)
                {
                    printf("ioctlsocket failed with error.");
                    continue;
                }
                lastIndex++;
                printf("New client request accepted (%d). Client address: %s : %d\n", lastIndex, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

            }
        }
        else
        {

            // Check if new message is received from connected clients
            for (int i = 0; i < lastIndex; i++)
            {
                // Check if new message is received from client on position "i"
                if (FD_ISSET(clientSockets[i], &readfds))
                {
                    iResult = recv(clientSockets[i], dataBuffer, BUFFER_SIZE, 0);

                    if (iResult > 0)
                    {
                        dataBuffer[iResult] = '\0';
                        printf("Message received from client (%d):\n", i + 1);
                        printf("Client send: %s\n", dataBuffer);
                        printf("_______________________________  \n");
                    }
                    else if (iResult == 0)
                    {
                        // connection was closed gracefully
                        printf("Connection with client (%d) closed.\n", i + 1);
                        closesocket(clientSockets[i]);

                        // sort array and clean last place
                        for (int j = i; j < lastIndex - 1; j++)
                        {
                            clientSockets[j] = clientSockets[j + 1];
                        }
                        clientSockets[lastIndex - 1] = 0;

                        lastIndex--;
                    }
                    else
                    {
                        // there was an error during recv
                        printf("recv failed with error: %d\n", WSAGetLastError());
                        closesocket(clientSockets[i]);

                        // sort array and clean last place
                        for (int j = i; j < lastIndex - 1; j++)
                        {
                            clientSockets[j] = clientSockets[j + 1];
                        }
                        clientSockets[lastIndex - 1] = 0;

                        lastIndex--;
                    }
                }
            }
        }
    }

    //Close listen and accepted sockets
    closesocket(listenSocket);


    // Deinitialize WSA library
    WSACleanup();

    return 0;

}
