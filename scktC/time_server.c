#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 // windows vista ke saath compatibility ke liye
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // ensure karna bhencho ki Winsock library linked hai
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET) // windows ke liye socket check kar raha hoon
#define CLOSESOCKET(s) closesocket(s) // windows me closesocket() use hota hai
#define GETSOCKETERRNO() (WSAGetLastError()) // windows ka error retriever
#else
#define ISVALIDSOCKET(s) ((s) >= 0) // linux ke liye valid socket check kar raha hoon
#define CLOSESOCKET(s) close(s) // linux me close() use hota hai
#define GETSOCKETERRNO() (errno) // linux ka error retriever
#endif
// yahan tak maine macros define kr liye, taki cross-platform chal sake

#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
    #if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2,2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
    #endif

    printf("configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 use kar raha hoon
    hints.ai_socktype = SOCK_STREAM; // TCP socket bana raha hoon
    hints.ai_flags = AI_PASSIVE; // Server mode ke liye

    struct addrinfo *bind_address;
    int result = getaddrinfo(0, "8080", &hints, &bind_address);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(result));
        return 1;
    }

    printf("creating socket...\n");
    SOCKET socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        freeaddrinfo(bind_address);
        return 1;
    }

    printf("Binding socket to local address...\n");
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        freeaddrinfo(bind_address);
        CLOSESOCKET(socket_listen);
        return 1;
    }
    freeaddrinfo(bind_address);

    printf("listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        CLOSESOCKET(socket_listen);
        return 1;
    }

    printf("waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen, (struct sockaddr*)&client_address, &client_len);
    if (!ISVALIDSOCKET(socket_client)) {
        fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
        CLOSESOCKET(socket_listen);
        return 1;
    }
    printf("Client connected\n");

    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    printf("%s\n", address_buffer);

    printf("reading yo request bitch...\n"); // Client ki request padh raha hoon
    char request[1024];
    int bytes_received = recv(socket_client, request, sizeof(request) - 1, 0);
    if (bytes_received < 0) {
        fprintf(stderr, "recv() failed. (%d)\n", GETSOCKETERRNO());
        CLOSESOCKET(socket_client);
        CLOSESOCKET(socket_listen);
        return 1;
    }
    request[bytes_received] = '\0'; // Null-terminate the received data
    printf("received %d bytes:\n%s\n", bytes_received, request);

    printf("sending response yall\n"); // Client ko response bhej raha hoon
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Local time is: ";
    int bytes_sent = send(socket_client, response, strlen(response), 0);
    printf("sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
    printf("sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

    printf("closing connection...\n"); // Client connection band kar raha hoon
    CLOSESOCKET(socket_client);
    printf("closing listening socket...\n"); // Server band kar raha hoon
    CLOSESOCKET(socket_listen);

    #if defined(_WIN32)
    WSACleanup(); // Windows cleanup
    system("pause"); // Windows-specific pause
    #endif

    printf("finished.\n");
    return 0;
}