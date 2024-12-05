#ifndef COMMUNICATIONLAYER_H
#define COMMUNICATIONLAYER_H

// Include platform-specific networking headers
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
#include <unistd.h>

#include <stddef.h> // For size_t

// Function declarations
int createSocket();
void setupServerAddress(struct sockaddr_in *address, const char *ip, int port);
int connectToServer(int sock, const struct sockaddr_in *address);
void sendDataToServer(int sock, const char *message);
void receiveDataFromServer(int sock, char *buffer, size_t bufferSize);

// Platform-specific functions
void initializePlatform();
void cleanupPlatform();

#endif // COMMUNICATIONLAYER_H

