/*
** datagram client.
*/
#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
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

#define BUFFLEN 1024
#define PORT 41234

void main()
{
	WSADATA wsaData;
	struct sockaddr_in clientAddr;
	int iResult, sockfd, clientLen, i = 0;
	char string[BUFFLEN];

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	/*Create Client socket*/
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket failed:");
		exit(0);
	}

	/* Clear Client structure */

	memset((char *)&clientAddr, 0, sizeof(struct sockaddr_in));
	clientAddr.sin_family = AF_INET;			        //For IPV4 internet protocol
	clientAddr.sin_port = htons(PORT);			    //Assign port number on which server listening	
	clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//Assign server machine address

	clientLen = sizeof(clientAddr);

	/* Send data to server */
	while (1)
	{
		printf("Enter any string\n");
		scanf_s("%s", string);

		/* Send string/data to server */
		if (sendto(sockfd, string, BUFFLEN, 0, (struct sockaddr_in *)&clientAddr, clientLen) == -1)
		{
			perror("sendto failed:");
			exit(1);
		}
	}

	closesocket(sockfd);
	WSACleanup();

}