#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

/*
Server Socket Microsoft tutorial


*/
//main
int main() {
	WSADATA wsaData;

	SOCKET ClientSocket = INVALID_SOCKET;;
	SOCKET ListenSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	/*
	struct addrinfo {
		int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
		int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
		int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
		int              ai_protocol;  // use 0 for "any"
		size_t           ai_addrlen;   // size of ai_addr in bytes
   |--->struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
   |	char            *ai_canonname; // full canonical hostname
   |	struct addrinfo *ai_next;      // linked list, next node
   |};
   |
   |-->struct sockaddr {
	unsigned short    sa_family;    // address family, AF_xxx
	char              sa_data[14];  // 14 bytes of protocol address
	};

	// (IPv4 only--see struct sockaddr_in6 for IPv6)

	struct sockaddr_in {
	short int          sin_family;  // Address family, AF_INET
	unsigned short int sin_port;    // Port number
	struct in_addr     sin_addr;    // Internet address
	unsigned char      sin_zero[8]; // Same size as struct sockaddr
	};
	// (IPv4 only--see struct in6_addr for IPv6)

	// Internet address (a structure for historical reasons)
	struct in_addr {
	uint32_t s_addr; // that's a 32-bit int (4 bytes)
	};
	// (IPv6 only--see struct sockaddr_in and struct in_addr for IPv4)

	struct sockaddr_in6 {
	u_int16_t       sin6_family;   // address family, AF_INET6
	u_int16_t       sin6_port;     // port number, Network Byte Order
	u_int32_t       sin6_flowinfo; // IPv6 flow information
	struct in6_addr sin6_addr;     // IPv6 address
	u_int32_t       sin6_scope_id; // Scope ID
	};

	struct in6_addr {
	unsigned char   s6_addr[16];   // IPv6 address
	};

	// (IPv6 AND IPv4) 
	struct sockaddr_storage {
	sa_family_t  ss_family;     // address family

	// all this is padding, implementation specific, ignore it:
	char      __ss_pad1[_SS_PAD1SIZE];
	int64_t   __ss_align;
	char      __ss_pad2[_SS_PAD2SIZE];
	};
	*/

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	/* Protocol Families
	The socket API is a general interface for Unix networking and allows the use of various network protocols and addressing architectures.
	The following lists a sampling of protocol families (preceded by the standard symbolic identifier) defined in a modern Linux or BSD implementation:
	Local to host (pipes and file-domain)				PF_LOCAL, PF_UNIX, PF_FILE
	Internet Protocol version 4							PF_INET
	Amateur Radio AX.25									PF_AX25
	Novell's Internetwork Packet Exchange				PF_IPX
	Appletalk											PF_APPLETALK
	Amateur radio NetROM (related to AX.25)				PF_NETROM
	Multiprotocol bridge								PF_BRIDGE
	Asynchronous Transfer ModePermanentVirtualCircuits	PF_ATMPVC
	Asynchronous Transfer ModeSwitchedVirtualCircuits	PF_ATMSVC
	Internet Protocol version 6							PF_INET6
	Reserved for DECnet project							PF_DECnet
	Reserved for 802.2LLC project						PF_NETBEUI
	Security callback pseudo AF							PF_SECURITY
	PF_KEY key management API							PF_KEY
	routing API											PF_NETLINK, PF_ROUTE
	Packet capture sockets								PF_PACKET
	Acorn Econet										PF_ECONET
	Linux IBM Systems Network Architecture(SNA)Project	PF_SNA
	IrDA sockets										PF_IRDA
	PPP over X sockets									PF_PPPOX
	Sangoma Wanpipe API sockets							PF_WANPIPE
	Bluetooth sockets									PF_BLUETOOTH

	**using the prefix AF instead of PF. The AF-identifiers are intended for all data structures that specifically deal with the address type and not the protocol family.
	*The POSIX.1—2008 specification doesn't specify any PF-constants, but only AF-constants
	*/

	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	
	// Create a SOCKET for the server to listen for client connections
	/*
	The function socket() creates an endpoint for communication and returns a file descriptor for the socket.
	socket() takes three arguments:
	-domain, which specifies the protocol family of the created socket. For example: 
	*AF_INET for network protocol IPv4 (IPv4-only)
	*AF_INET6 for IPv6 (and in some cases, backward compatible with IPv4)
	*AF_UNIX for local socket (using a file)
	-type, one of:
	*SOCK_STREAM (reliable stream-oriented service or Stream Sockets)
	*SOCK_DGRAM (datagram service or Datagram Sockets)
	*SOCK_SEQPACKET (reliable sequenced packet service)
	*SOCK_RAW (raw protocols atop the network layer)
	protocol specifying the actual transport protocol to use.
	The most common are IPPROTO_TCP, IPPROTO_SCTP, IPPROTO_UDP, IPPROTO_DCCP.
	These protocols are specified in file netinet/in.h.
	The value 0 may be used to select a default protocol from the selected domain and type.
	The function returns -1 if an error occurred. Otherwise, it returns an integer representing the newly assigned descriptor.
	*/
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	// Setup the TCP listening socket
	/*Once you have a socket, you might have to associate that socket with a port on your local machine. 
	bind() assigns a socket to an address. 
	When a socket is created using socket(), it is only given a protocol family,
	but not assigned an address. This association with an address must be performed with the bind()
	system call before the socket can accept connections to other hosts. 
	bind() takes three arguments:
	*sockfd, a descriptor representing the socket to perform the bind on.
	*my_addr, a pointer to a sockaddr structure representing the address to bind to.
	*addrlen, a socklen_t field specifying the size of the sockaddr structure.
	Bind() returns 0 on success and -1 if an error occurs.
	*/
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);


	/*
	After a socket has been associated with an address, 
	listen() prepares it for incoming connections. 
	However, this is only necessary for the stream-oriented (connection-oriented) data modes, i.e.,
	for socket types (SOCK_STREAM, SOCK_SEQPACKET). 
	listen() requires two arguments:
	*sockfd, a valid socket descriptor.
	*backlog, an integer representing the number of pending connections that can be queued up at any one time. 
	The operating system usually places a cap on this value.
	Once a connection is accepted, it is dequeued. 
	On success, 0 is returned. If an error occurs, -1 is returned.
	*/
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	

	// Accept a SINGLE client socket
	/*
	When an application is listening for stream-oriented connections from other hosts, 
	it is notified of such events (cf. select() function) and must initialize the connection using the accept() function.
	The accept() function creates a new socket for each connection and removes the connection from the listen queue.
	It takes the following arguments:
	*sockfd, the descriptor of the listening socket that has the connection queued.
	*cliaddr, a pointer to a sockaddr structure to receive the client's address information.
	*addrlen, a pointer to a socklen_t location that specifies the size of the client address structure passed to accept(). 
	When accept() returns, this location indicates how many bytes of the structure were actually used.
	The accept() function returns the new socket descriptor for the accepted connection, or -1 if an error occurs. 
	All further communication with the remote host now occurs via this new socket.
	Datagram sockets do not require processing by accept() since the receiver may immediately respond to the request using the listening socket.
	*/
	
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	// No longer need server socket
	closesocket(ListenSocket);

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);

			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}
