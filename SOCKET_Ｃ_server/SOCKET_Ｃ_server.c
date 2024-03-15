#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

const char* host = "0.0.0.0";
int port = 7000;

int main()
{
    SOCKET sock, new_sock;
    socklen_t addrlen;
    struct sockaddr_in my_addr, client_addr;
    int status;
    char indata[1024] = { 0 }, outdata[1024] = { 0 };
    int on = 1;

    // init winsock
    WSADATA wsa = { 0 };
    WORD wVer = MAKEWORD(2, 2);
    WSAStartup(wVer, &wsa);
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != NO_ERROR) {
        printf("Error: init winsock\n");
        exit(1);
    }

    // create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("Socket creation error");
        exit(1);
    }

    // for "Address already in use" error message
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) == -1) {
        perror("Setsockopt error");
        exit(1);
    }

    // server address
    my_addr.sin_family = AF_INET;
    inet_pton(AF_INET, host, &my_addr.sin_addr);
    my_addr.sin_port = htons(port);

    status = bind(sock, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (status == -1) {
        perror("Binding error");
        exit(1);
    }
    char my_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &my_addr.sin_addr, my_ip, sizeof(my_ip));
    printf("server start at: %s:%d\n", my_ip, port);

    status = listen(sock, 5);
    if (status == -1) {
        perror("Listening error");
        exit(1);
    }
    printf("wait for connection...\n");

    addrlen = sizeof(client_addr);

    while (1) {
        new_sock = accept(sock, (struct sockaddr*)&client_addr, &addrlen);
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("connected by %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        while (1) {
            int nbytes = recv(new_sock, indata, sizeof(indata), 0);
            if (nbytes <= 0) {
                closesocket(new_sock);
                printf("client closed connection.\n");
                break;
            }
            printf("recv: %s\n", indata);

            sprintf(outdata, "echo %s", indata);
            send(new_sock, outdata, strlen(outdata), 0);
        }
    }
    closesocket(sock);
    WSACleanup();

    return 0;
}