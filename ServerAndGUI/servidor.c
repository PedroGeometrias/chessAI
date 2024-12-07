/*
* Servidor de comunicação interface/GUI
* autor : Pedro Haro
* motivo : envio de messagems tipo FEN para interface e a GUI do xadrez
* permitindo atualização do GUI conforme respostas da IA
*/

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define close closesocket
#else
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define PORT 8080
#define BUFF_SIZE 100

// define
int serverSocket = -1;

// prototipos
void initializePlatform();
void cleanupPlatform();
int createServerSocket();
void setupServerAddress(struct sockaddr_in *address, int port);
int bindSocket(int serverSocket, const struct sockaddr_in *address);
int listenForConnections(int serverSocket);
int acceptConnection(int serverSocket);
void receiveData(int clientSocket, char *buffer, size_t bufferSize);
void sendData(int clientSocket, const char *message);
void handleSignal(int sig);
int handleClientCommunication(int fromClientSocket, int toClientSocket, char *buffer);

int main() {

    // inicializo a plataforma, no windows se tem de inicializar um socket
    initializePlatform();  

    // sinal de interrupcao
    signal(SIGINT, handleSignal);

    // creando o socket
    printf("Inicializando servidor...\n");
    serverSocket = createServerSocket();

    // temos 2 clientes, o GUI e o intermediateTranslator.c
    int clientSocket1 = acceptConnection(serverSocket);
    int clientSocket2 = acceptConnection(serverSocket);

    // loop que controla a comunicacao
    while (1) {
        // string enviada de um cliente ao outro
        char fen[BUFF_SIZE];
        if (handleClientCommunication(clientSocket1, clientSocket2, fen) == -1) break;
        if (handleClientCommunication(clientSocket2, clientSocket1, fen) == -1) break;
    }

    // fechando os sockets
    close(clientSocket1);
    close(clientSocket2);
    close(serverSocket);

    // limpando no windows
    cleanupPlatform();  
    return 0;
}

// inicializando no windows 
void initializePlatform() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "Failed to initialize Winsock. Error: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
#endif
}

// limpando de plataforma especifica ( windows )
void cleanupPlatform() {
#ifdef _WIN32
    WSACleanup();
#endif
}

// fechando server
void handleSignal(int sig) {
    printf("Received signal %d, shutting down...\n", sig);
    if (serverSocket != -1) {
        close(serverSocket);
    }
    cleanupPlatform();
    exit(0);
}

// criando e setando
int createServerSocket() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
#ifdef _WIN32
        fprintf(stderr, "Socket creation failed. Error: %d\n", WSAGetLastError());
#else
        perror("socket");
#endif
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress;
    setupServerAddress(&serverAddress, PORT);
    bindSocket(serverSocket, &serverAddress);
    listenForConnections(serverSocket);

    printf("Server is running on port %d. Waiting for client connections...\n", PORT);
    return serverSocket;
}

// structure address
void setupServerAddress(struct sockaddr_in *address, int port) {
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    address->sin_addr.s_addr = INADDR_ANY;
}

// socket
int bindSocket(int serverSocket, const struct sockaddr_in *address) {
    if (bind(serverSocket, (struct sockaddr *)address, sizeof(*address)) == -1) {
#ifdef _WIN32
        fprintf(stderr, "Bind failed. Error: %d\n", WSAGetLastError());
#else
        perror("bind");
#endif
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    return 0;
}

// conexao
int listenForConnections(int serverSocket) {
    if (listen(serverSocket, 5) == -1) {
#ifdef _WIN32
        fprintf(stderr, "Listen failed. Error: %d\n", WSAGetLastError());
#else
        perror("listen");
#endif
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    return 0;
}

int acceptConnection(int serverSocket) {
    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1) {
#ifdef _WIN32
        fprintf(stderr, "Accept failed. Error: %d\n", WSAGetLastError());
#else
        perror("accept");
#endif
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted from client\n");
    return clientSocket;
}

// dados sao recebidos com essa funcao
void receiveData(int clientSocket, char *buffer, size_t bufferSize) {
    ssize_t bytesReceived = recv(clientSocket, buffer, bufferSize - 1, 0);
    if (bytesReceived == -1) {
#ifdef _WIN32
        fprintf(stderr, "Receive failed. Error: %d\n", WSAGetLastError());
#else
        perror("recv");
#endif
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    buffer[bytesReceived] = '\0';

    if (bytesReceived >= bufferSize - 1) {
        fprintf(stderr, "Received message too large for the buffer\n");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
}

// enviado dados
void sendData(int clientSocket, const char *message) {
    if (send(clientSocket, message, strlen(message), 0) == -1) {
#ifdef _WIN32
        fprintf(stderr, "Send failed. Error: %d\n", WSAGetLastError());
#else
        perror("send");
#endif
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
}

// comunicacao
int handleClientCommunication(int fromClientSocket, int toClientSocket, char *buffer) {
    memset(buffer, 0, BUFF_SIZE);
    receiveData(fromClientSocket, buffer, BUFF_SIZE);
    if (strlen(buffer) > 0) {
        sendData(toClientSocket, buffer);
        return 0;
    }
    return -1;
}

