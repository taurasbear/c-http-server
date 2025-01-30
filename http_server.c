#include "http_server.h"
#include <stdio.h>

void initialize_winsock(WSADATA *wsadata)
{
    int iResult = WSAStartup(MAKEWORD(2, 2), wsadata);
    if (iResult != 0)
    {
        fprintf(stderr, "WSAStartup failed: %d,", iResult);
        exit(1);
    }
}

SOCKET create_listen_socket(struct addrinfo *result)
{
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Failed to create socket: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }
    return ListenSocket;
}

void bind_socket(SOCKET ListenSocket, struct addrinfo *result)
{
    int iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to bind socket: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        exit(1);
    }
}

void listen_on_socket(SOCKET ListenSocket)
{
    int iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to listen on socket: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        exit(1);
    }
}

SOCKET accept_client(SOCKET ListenSocket)
{
    SOCKET ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Failed to accept client: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        exit(1);
    }
}

void shutdown_client(SOCKET ClientSocket)
{
    int iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to shutdown client: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        exit(1);
    }
}

void handle_client(SOCKET ClientSocket)
{
    // TODO: Implement or remove
    exit(1);
}