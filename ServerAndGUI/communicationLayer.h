#ifndef COMMUNICATIONLAYER_H
#define COMMUNICATIONLAYER_H

// Include platform-specific networking headers
#ifdef _WIN32
#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <stddef.h> 
// Other common headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

