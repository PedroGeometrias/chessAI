#include "communicationLayer.h"


// criacao do socket
int createSocket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sock;
}

// setacando a structure address com o ip local e a porta
void setupServerAddress(struct sockaddr_in *address, const char *ip, int port) {
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &address->sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
}

// conectando no servidor
int connectToServer(int sock, const struct sockaddr_in *address) {
    printf("Attempting to connect to the server...\n");
    if (connect(sock, (struct sockaddr *)address, sizeof(*address)) == -1) {
        perror("connect");
        printf("Failed to connect to the server.\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server successfully.\n");
    return 0;
}


// enviando os dados para o servidor no socket
void sendDataToServer(int sock, const char *message) {
    if (send(sock, message, strlen(message), 0) == -1) {
        perror("send");
        close(sock);
        exit(EXIT_FAILURE);
    }
}

// recebendo dados
void receiveDataFromServer(int sock, char *buffer, size_t bufferSize) {
    ssize_t bytesReceived = recv(sock, buffer, bufferSize - 1, 0);
    if (bytesReceived == -1) {
        perror("recv");
        close(sock);
        exit(EXIT_FAILURE);
    }
    buffer[bytesReceived] = '\0';
}

// FUNCOES DO WINDOWS //

void initializePlatform() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "ERROR INITIALIZING WINSOCK: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
#endif
}

void cleanupPlatform() {
#ifdef _WIN32
    WSACleanup();
#endif
}

