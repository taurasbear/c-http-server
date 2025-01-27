#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char *argv[])
{
    printf("Hello world from client!");

    return 0;
}