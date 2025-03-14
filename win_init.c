#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main(){
    WSADATA d;

    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        printf("failed to initialize.\n");
        return -1;
    }

    WSACleanup();
    printf("Ok.\n");
    system("pause");
    return 0;

}

//gcc win_init.c -o win_init.exe -lws2_32