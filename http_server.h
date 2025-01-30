#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <WinSock2.h>
#include <WS2tcpip.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUF_LEN 128

typedef struct HttpRequest
{
    char method[16];
    char path[256];
    char protocol[16];
    char headers[20][256];
    char body[1024];

} HttpRequest;

void initialize_winsock(WSADATA *wsadata);
SOCKET create_listen_socket(struct addrinfo *result);
void bind_socket(SOCKET ListenSocket, struct addrinfo *result);
void listen_on_socket(SOCKET ListenSocket);
SOCKET accept_client(SOCKET ListenSocket);
void shutdown_client(SOCKET ClientSocket);
void handle_client(SOCKET ClientSocket);

#endif