# Network Adapter Address Linked List

This project demonstrates how to use **Windows Sockets (Winsock)** and the **IP Helper API** to retrieve and display details about network adapters and their unicast addresses in a linked list format. It is implemented in **C** for Windows systems.

## Features
- Initializes Winsock for network operations.
- Uses `GetAdaptersAddresses` API to fetch network adapter details.
- Displays adapter names and their associated unicast IP addresses (IPv4/IPv6).
- Dynamically allocates memory for adapters and handles errors gracefully.

## Dependencies
Ensure the following libraries are linked during compilation:
- `ws2_32.lib` (Winsock library)
- `iphlpapi.lib` (IP Helper API library)

## How to Compile
For **MinGW**, use the following command to compile the code:
```bash
gcc win_list.c -o win_list.exe -liphlpapi -lws2_32
