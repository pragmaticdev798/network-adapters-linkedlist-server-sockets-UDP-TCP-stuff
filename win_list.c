// implementing a linked list on network adapter addresses
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600  // yahan maine macro define kia hai... 0x0600 yahan pe minimum windows version hai jo supported hai
#endif

#include <winsock2.h>
#include <iphlpapi.h> // getting IP Helper APIs for routing tables, details in sab ke liye
#include <ws2tcpip.h> // advanced tcp/ip version pe work krne ke liye
#include <stdio.h>  // input/output ke liye 
#include <stdlib.h>  // memory manage krne ke liye

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib") // mingw ke liye gcc win_list.c -o win_list.exe -liphlpapi -lws2_32

int main() {
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {   // winsock initialize
        printf("Failed to initialize.\n");
        return -1;
    }

    // Allocating memory to adapters
    DWORD asize = 20000;
    PIP_ADAPTER_ADDRESSES adapters;

    do {
        adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize);
        if (!adapters) {
            printf("Couldn't allocate %ld bytes for adapters.\n", asize);
            WSACleanup();
            return -1;
        }

        int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0, adapters, &asize);
        if (r == ERROR_BUFFER_OVERFLOW) { // adjusting memory size if buffer overflows
            printf("GetAdaptersAddresses wants %ld bytes.\n", asize);
            free(adapters);
        } else if (r == ERROR_SUCCESS) {
            break;
        } else {
            printf("Error from GetAdaptersAddresses: %d\n", r);
            free(adapters);
            WSACleanup();
            return -1;
        }
    } while (!adapters);

    PIP_ADAPTER_ADDRESSES adapter = adapters;
    while (adapter) {
        printf("\nAdapter name: %S\n", adapter->FriendlyName); // adapter name output
        PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;

        while (address) { // loop through unicast addresses
            printf("\t%s", address->Address.lpSockaddr->sa_family == AF_INET ? "IPV4" : "IPV6");

            char ap[100]; // storage for IP address
            getnameinfo(address->Address.lpSockaddr, 
                        address->Address.iSockaddrLength, 
                        ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
            printf("\t%s\n", ap);

            address = address->Next;
        }

        adapter = adapter->Next;
    }

    free(adapters); // freeing allocated memory
    WSACleanup(); // cleaning up winsock
    system("pause");

    return 0;

}

// gcc win_list.c -o win_list.exe -liphlpapi -lws2_32

