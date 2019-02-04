
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
	int fd;
	int rv;
	
	// Get info about the address
	struct addrinfo hints;
	struct addrinfo* result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
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
		
		fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (fd < 0)
		{
			// Create socket failed
			printf("Create socket failed\n");
			continue;
		}
		
		printf("Connecting... ");
		rv = connect(fd, r->ai_addr, r->ai_addrlen);
		if (rv < 0)
		{
			// Connect failed
			printf("failed\n");
			close(fd);
			continue;
		}
		
		printf("OK\n");
		break;
	}
	
	freeaddrinfo(result);
	
	if (!r)
	{
		// All available result failed connecting
		// TODO: properly handle
		printf("Connect failed to all addrinfos.\n");
		exit(-1);
	}
	
	char buffer[256] = "Hello world!";
	rv = write(fd, buffer, strlen(buffer));
	if (rv < 0)
	{
		// Write failed
		// TODO: handle
		printf("Write failed.\n");
		close(fd);
		exit(-1);
	}
	
	memset(buffer, 0, sizeof(buffer));
	rv = read(fd, buffer, sizeof(buffer));
	if (rv < 0)
	{
		// Read failed
		// TODO: handle
		printf("Read failed.\n");
		close(fd);
		exit(-1);
	}
	
	printf("%s\n", buffer);
	
	close(fd);
	
	return 0;
}
