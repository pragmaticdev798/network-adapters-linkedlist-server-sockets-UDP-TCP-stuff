#if defined (_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#include <stdio.h>

int main() {

    #if defined (_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2,2), &d)){
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
    #endif
        system("pause");
        return 0;

}