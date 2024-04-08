#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP "172.217.164.78"
#define SERVER_PORT 80

int initialize_winsock() {
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        fprintf(stderr, "wsa_startup failed: %d\n", result);
        return 0;
    }
    return 1;
}

SOCKET create_socket() {
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        fprintf(stderr, "can't create socket\n");
    }
    return client_socket;
}

int connect_to_server(SOCKET client_socket, const char* server_ip, unsigned short server_port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "invalid address/address not supported\n");
        return 0;
    }
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        fprintf(stderr, "connection failed\n");
        return 0;
    }
    return 1;
}

int send_http_request(SOCKET client_socket, const char* request) {
    int result = send(client_socket, request, strlen(request), 0);
    if (result == SOCKET_ERROR) {
        fprintf(stderr, "send failed\n");
        return 0;
    }
    return 1;
}

void receive_response(SOCKET client_socket) {
    char buffer[1024];
    int bytes_received;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, stdout);
    }
    if (bytes_received == SOCKET_ERROR) {
        fprintf(stderr, "receive failed\n");
    }
}

void cleanup_socket(SOCKET client_socket) {
    closesocket(client_socket);
}

void cleanup_winsock() {
    WSACleanup();
}

int main() {
    if (!initialize_winsock())
        return EXIT_FAILURE;

    SOCKET client_socket = create_socket();
    if (client_socket == INVALID_SOCKET) {
        cleanup_winsock();
        return EXIT_FAILURE;
    }

    if (!connect_to_server(client_socket, SERVER_IP, SERVER_PORT)) {
        cleanup_socket(client_socket);
        cleanup_winsock();
        return EXIT_FAILURE;
    }

    const char* request = "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n";
    if (!send_http_request(client_socket, request)) {
        cleanup_socket(client_socket);
        cleanup_winsock();
        return EXIT_FAILURE;
    }

    receive_response(client_socket);

    cleanup_socket(client_socket);
    cleanup_winsock();

    return EXIT_SUCCESS;
}