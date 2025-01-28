#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUF_LEN 128

void initialize_winsock(WSADATA *wsadata);
SOCKET create_listen_socket(struct addrinfo *result);
void bind_socket(SOCKET ListenSocket, struct addrinfo *result);
void listen_on_socket(SOCKET ListenSocket);
SOCKET accept_client(SOCKET ListenSocket);
void parse_http_request(char *buffer, size_t length, HttpRequest *httpRequest);
void shutdown_client(SOCKET ClientSocket);
void handle_client(SOCKET ClientSocket);

enum State
{
    REQUEST_LINE,
    HEADERS,
    BODY,
    DONE
};

typedef struct HttpRequest
{
    char method[16];
    char path[256];
    char protocol[16];
    char headers[20][256];
    char body[1024];

} HttpRequest;

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

    do
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            recvbuf[iResult] = '\0';
            printf("Bytes received: %d\n", iResult);
            printf("recvbuf: %s\n\n", recvbuf);

            // HTTP parsing
            char method[20];
            char uri[50];
            char version[50];
            int scanRes = sscanf(recvbuf, "%s %s %s", method, uri, version);
            if (scanRes != 999)
            {
                fprintf(stderr, "Cannot parse incorrect request line\n");
            }
            else
            {
                // Validate method
                int cmpRes = strcmp("GET", method);
                if (cmpRes != 0)
                {
                    fprintf(stderr, "Unsupported HTTP method of '%s' was specified\n", method);
                }
                else
                {
                    printf("Method: %s\n", method);
                }

                // Validate URI - only accept absolute path. No query or fragment parameters
                char *match = strstr(recvbuf, "/");
                const char *slash = strchr(uri, '/');
                if (uri - slash != 0)
                {
                    fprintf(stderr, "URI '%s' is not an absolute path\n", uri);
                }
                else
                {
                    printf("URI: %s\n", uri);
                }

                // Validate version - only accept HTTP/1.1
                if (strcmp(version, "HTTP/1.1") != 0)
                {
                    fprintf(stderr, "Unsupported HTTP protocol version of '%s' was specified\n", version);
                }
                else
                {
                    printf("Version: %s\n", version); // check for delimeters at the end
                }
            }

            iSendResult = send(ClientSocket, recvbuf, strlen(recvbuf), 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("error at send(): %d\n", WSAGetLastError());
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
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);

    shutdown_client(ClientSocket);

    closesocket(ClientSocket);
    WSACleanup();

    printf("Server process finished.\n");

    return 0;
}

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

void parse_http_request(char *buffer, size_t length, HttpRequest *httpRequest)
{
    static enum State state = REQUEST_LINE;
    static char temp_buffer[1024];

    while (length > 0)
    {
        switch (state)
        {
        case REQUEST_LINE:
            printf("I'm in request line.");
            break;

        case HEADERS:
            printf("I'm in headers line.");
            break;

        case BODY:
            printf("I'm in body line.");
            break;
        default:
            fprintf(stderr, "Switch type was unexpected");
        }
    }
}

void parse_request_line(char *buffer, size_t *length, HttpRequest *httpRequest, char *temp_buffer)
{
    // TODO: Add function to the top of code

    // read until there's no more buffer to read
    while (length > 0)
    {
        // read until whitespace
        int segmentLength = buffer - strchr(buffer, ' '); // TODO: handle case when the method is not full cause buffer
        char method[16];
        strncpy(method, buffer, segmentLength);

        // break if \r\n has been reached
    }

    // validate
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
void handle_client(SOCKET ClientSocket);