#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUF_LEN 128

int main(int argc, char *argv[])
{
    printf("Hello world from client!\n");

    WSADATA wsadata;

    SOCKET ConnectSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    int recvbuflen = DEFAULT_BUF_LEN;
    const char *sendbuf = "GET /index.html HTTP/1.1\r\n";
    char recvbuf[DEFAULT_BUF_LEN];
    int iResult;

    if (argc != 2)
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server.\n");
        WSACleanup();
        return 1;
    }

    // iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    // if (iResult == SOCKET_ERROR)
    // {
    //     printf("error at send(): %d\n", WSAGetLastError());
    //     closesocket(ConnectSocket);
    //     WSACleanup();
    //     return 1;
    // }
    // printf("Bytes sent: %d\n", iResult);

    // ---Testing
    const char *sendbuf1 = "GE";
    iResult = send(ConnectSocket, sendbuf1, (int)strlen(sendbuf1), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("error at send(sendbuf1): %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Bytes sent: %d\n", iResult);

    const char *sendbuf2 = "T /index";
    iResult = send(ConnectSocket, sendbuf2, (int)strlen(sendbuf2), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("error at send(sendbuf2): %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Bytes sent: %d\n", iResult);
    
    const char *sendbuf3 = ".html HTTP/1.1\r\n";
    iResult = send(ConnectSocket, sendbuf3, (int)strlen(sendbuf3), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("error at send(sendbuf3): %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Bytes sent: %d\n", iResult);
    
    // Testing---

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    do
    {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            printf("Bytes received: %d\n", iResult);
        }
        else if (iResult == 0)
        {
            printf("Connection closing...\n");
        }
        else
        {
            printf("error at recv(): %d\n", WSAGetLastError());
        }
    } while (iResult > 0);

    closesocket(ConnectSocket);
    WSACleanup();

    printf("Client process finished.\n");

    return 0;
}