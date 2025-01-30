#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include "http_server.h"
#include "http_parser.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char *argv[])
{
    printf("Hello world from server!\n");

    WSADATA wsadata;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int recvbuflen = DEFAULT_BUF_LEN;
    char recvbuf[DEFAULT_BUF_LEN];

    int iResult;
    int iSendResult;

    initialize_winsock(&wsadata);

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    ListenSocket = create_listen_socket(result);

    bind_socket(ListenSocket, result);

    freeaddrinfo(result);

    listen_on_socket(ListenSocket);

    ClientSocket = accept_client(ListenSocket);

    closesocket(ListenSocket);

    HttpRequest *httpRequest = (HttpRequest *)malloc(sizeof(httpRequest));
    if (httpRequest == NULL)
    {
        fprintf(stderr, "Failed allocate memory for HttpRequest struct");
        return 1;
    }
    do
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            recvbuf[iResult] = '\0';
            printf("Bytes received: %d\n", iResult);
            printf("recvbuf: %s\n\n", recvbuf);

            parse_http_request(recvbuf, iResult, httpRequest);

            iSendResult = send(ClientSocket, recvbuf, strlen(recvbuf), 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("error at send(): %d\n", WSAGetLastError());
                free(httpRequest);
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        }
        else if (iResult == 0)
        {
            printf("Connection closing...\n");
        }
        else
        {
            printf("error at recv(): %d\n", WSAGetLastError());
            free(httpRequest);
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);

    shutdown_client(ClientSocket);

    free(httpRequest);
    closesocket(ClientSocket);
    WSACleanup();

    printf("Server process finished.\n");

    return 0;
}