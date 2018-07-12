#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char *argv[])
{

	// Declare some variables
	WSADATA wsaData;

	int iResult = 0;            // used to return function results

								// the listening socket to be created
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET SendSocket = INVALID_SOCKET;
	// The socket address to be passed to bind
	sockaddr_in service;
	sockaddr_in RecvAddr;

	unsigned short Port = 50001;
	const int MAXBUFLEN = 65514;
	const int BUFFER_SIZE = 65515;	//The maximum number of bytes to receive
	char buffer[MAXBUFLEN];	//Storage of the received data
	
	int BufLen = 1024;	//The maximum number of bytes to send
	char SendBuf[1024];	//Storage of the received data
	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"Error at WSAStartup()\n");
		return 1;
	}
	//----------------------
	// Create a SOCKET for listening for 
	// incoming connection requests
	// Create a socket for sending data
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	ListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ListenSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		closesocket(ListenSocket);
		closesocket(SendSocket);
		return 1;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	service.sin_family = AF_INET;
	service.sin_port = htons(58947);
	if (argc > 3) {
		printf("Service address: %s:%d", argv[3], service.sin_port);
		service.sin_addr.s_addr = inet_addr(argv[3]);
	}
	else
		service.sin_addr.s_addr = inet_addr("127.0.0.1");//inet_pton(PF_INET, "0.0.0.0", &(service.sin_addr)); // inet_pton("127.0.0.1"); //inet_addr
	

	//----------------------
	// Bind the socket.
	iResult = bind(SendSocket, (SOCKADDR *)&service, sizeof(service));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"\nbind failed with error %u\n", WSAGetLastError());
		closesocket(ListenSocket);
		closesocket(SendSocket);
		WSACleanup();
		return 3;
	}
	else
		printf(" -> Bound.\n");


	//---------------------------------------------
	// Set up the RecvAddr structure with the IP address of
	// the receiver (in this example case "192.168.1.1")
	// and the specified port number.
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = inet_addr(argv[1]);//inet_pton(AF_INET, argv[1], &(service.sin_addr));

	iResult = sendto(SendSocket, argv[2], strlen(argv[2]), 0, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
		closesocket(SendSocket);
		closesocket(ListenSocket);
		WSACleanup();
		return 4;
	}
	
	printf("talker: sent %d bytes to %s:%d\n", iResult, argv[1], Port);
	int server_length = sizeof(struct sockaddr_in);
	/* Receive bytes from client */
	int flag = 1;
	int size;
	while (1) {
		iResult = recvfrom(SendSocket, buffer, MAXBUFLEN - 1, 0, (SOCKADDR *)& RecvAddr, &server_length);
		if (iResult < 0) {
			//none
		}
		else {
			printf("listener: packet is %d bytes long\n", iResult);
			buffer[iResult] = '\0';
			/*
			int i;
			for (i = 0; i < iResult; i++)
			{
				if (i > 0) printf(":");
				printf("%02X", buffer[i]);
			}
			*/
			printf("\n");
			printf("Recive UDP: \"%s\"\n", buffer);
			break;
		}
	}


	//---------------------------------------------
	// When the application is finished sending, close the socket.
	iResult = closesocket(SendSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 5;
	}
	iResult = closesocket(ListenSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 6;
	}
	wprintf(L"Sockets closed.\n");
	//---------------------------------------------

	WSACleanup();
	return 0;
}