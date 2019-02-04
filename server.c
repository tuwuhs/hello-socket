
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, int* argv[])
{
	int serverfd;
	int rv;
	
	// Get info about the address
	struct addrinfo hints;
	struct addrinfo* result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	rv = getaddrinfo("127.0.0.1", "10000", &hints, &result);
	if (rv != 0)
	{
		// getaddrinfo failed
		// TODO: handle
		// Q: do we need to free result?
		printf("getaddrinfo() failed.\n");
		exit(-1);
	}
	
	// Loop through the resulting linked list
	// Connect to the first one that works
	struct addrinfo* r;
	for (r = result; r; r = r->ai_next)
	{
		// Assume r->sa_family == AF_INET, so we can cast to sockaddr_in
		struct sockaddr_in* addr_in = (struct sockaddr_in*) r->ai_addr;
		char* addrStr = inet_ntoa(addr_in->sin_addr);
		int port = ntohs(addr_in->sin_port);
		printf("--> %s:%d\n", addrStr, port);
		
		serverfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (serverfd < 0)
		{
			// Create socket failed
			printf("Create socket failed\n");
			continue;
		}
		
		int yes = 1;
		rv = setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		if (rv < 0)
		{
			// setsockopt() failed
			printf("setsockopt() failed\n");
			continue;
		}
		
		printf("Binding... ");
		rv = bind(serverfd, r->ai_addr, r->ai_addrlen);
		if (rv < 0)
		{
			// Connect failed
			printf("failed\n");
			close(serverfd);
			continue;
		}
		
		printf("OK\n");
		break;
	}
	
	freeaddrinfo(result);
	
	if (!r)
	{
		// All available result failed binding
		// TODO: properly handle
		printf("Bind failed to all addrinfos.\n");
		exit(-1);
	}
	
	rv = listen(serverfd, 1);
	if (rv < 0)
	{
		// Listen failed
		// TODO: handle
		printf("Listen failed.\n");
		close(serverfd);
		exit(-1);
	}
	printf("Listening...\n");
	
	int connfd;
	struct sockaddr peerAddr;
	int peerAddrSize = sizeof(peerAddr);
	connfd = accept(serverfd, (struct sockaddr*) &peerAddr, &peerAddrSize);
	if (connfd < 0)
	{
		// Accept failed
		// TODO: handle
		printf("Accept failed.\n");
		close(serverfd);
		exit(-1);
	}
	
	struct sockaddr_in* addr_in = (struct sockaddr_in*) &peerAddr;
	char* addrStr = inet_ntoa(addr_in->sin_addr);
	int port = ntohs(addr_in->sin_port);
	printf("Accepted connection from %s:%d\n", addrStr, port);
	
	// Close the listening socket to only allow one connection
	close(serverfd);
	
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	while (1)
	{
		rv = read(connfd, buffer, sizeof(buffer));
		if (rv == 0)
		{
			printf("Connection closed by peer\n");
			break;
		}
		else if (rv < 0)
		{
			// Read failed
			// TODO: handle
			printf("Read failed.\n");
			close(connfd);
			close(serverfd);
			exit(-1);
		}
		
		printf("%s\n", buffer);
		
		rv = write(connfd, buffer, strlen(buffer));
		if (rv < 0)
		{
			// Read failed
			// TODO: handle
			printf("Write failed.\n");
			close(connfd);
			close(serverfd);
			exit(-1);
		}
	}
	
	close(connfd);
	
	return 0;
}
