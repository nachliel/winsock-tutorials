/*
** server.c -- a stream socket server demo
*/
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define PORT_1 "27015"
#define PORT_2 "27016"


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	WSADATA wsaData;

	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ClientSocket2 = INVALID_SOCKET;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ListenSocket2 = INVALID_SOCKET;

	struct addrinfo *result = NULL, *ptr = NULL, hints;
	struct addrinfo* result2 = NULL;

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	printf("Starting WSA-DATA.\n");
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	// EAR ONE :
	// Resolve the local address and port to be used by the server
	printf("getting address...\n");
	iResult = getaddrinfo(NULL, PORT_1, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	printf("listenning...\n");
	// Listening 
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	printf("binding to port: %s\n", PORT_1);
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);
	printf("listtening...\n");
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	printf("accepting connections...\n");
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ListenSocket);
	
	
	// Ear Two SECOND PORT :
	// Resolve the local address and port to be used by the server
	printf("Openning new socket:\n");
	printf("getting address..\n");
	iResult = getaddrinfo(NULL, PORT_2, &hints, &result2);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Listening 
	printf("creating socket...\n");
	ListenSocket = socket(result2->ai_family, result2->ai_socktype, result2->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result2);
		WSACleanup();
		return 1;
	}
	printf("bind to port: %s\n",PORT_2);
	iResult = bind(ListenSocket, result2->ai_addr, (int)result2->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result2);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result2);
	printf("listtening...\n");
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	printf("accepting connections...\n");
	ClientSocket2 = accept(ListenSocket, NULL, NULL);
	if (ClientSocket2 == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	// No longer need server socket
	//closesocket(ListenSocket);
	// Receive until the peer shuts down the connection
	printf("Waitting for connection...");
	int iResult2;
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		iResult2 = recv(ClientSocket2, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Port 1: Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);

			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			return 1;
		}
		
		if (iResult2 > 0) {
			printf("Port 2: Bytes received: %d\n", iResult2);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket2, recvbuf, iResult2, 0);

			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket2);
				
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult2 == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket2);
			return 1;
		}
		

	} while (iResult > 0 || iResult2>0);
	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ClientSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	iResult = shutdown(ClientSocket2, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket2);
		WSACleanup();
		return 1;
	}
	// cleanup
	closesocket(ClientSocket);
	closesocket(ClientSocket2);
	WSACleanup();

	printf("Press ENTER key to Continue\n");
	getchar();
	return 0;
}