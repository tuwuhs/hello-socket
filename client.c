
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

	// Loop through the resulting linked list
	// Connect to the first one that works
	struct addrinfo* r;
	for (r = result; r; r = r->ai_next)
	{
		fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (fd < 0)
		{
			// Create socket failed
			continue;
		}
		
		struct sockaddr_in* addr_in = (struct sockaddr_in*) r->ai_addr;
		char* addrStr = inet_ntoa(addr_in->sin_addr);
		printf("Connecting to %s... ", addrStr);
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
	
	if (!r)
	{
		// All available result failed connecting
		// TODO: properly handle
		printf("Connect failed to all the resulting addrinfos.\n");
		freeaddrinfo(result);
		exit(-1);
	}
	
	freeaddrinfo(result);
	
	char buffer[256] = "Hello world!";
	rv = write(fd, buffer, strlen(buffer));
	if (rv < 0)
	{
		// Write failed
		// TODO: handle
		printf("Write failed.\n");
		exit(-1);
	}
	
	memset(buffer, 0, sizeof(buffer));
	rv = read(fd, buffer, sizeof(buffer));
	if (rv < 0)
	{
		// Read failed
		// TODO: handle
		printf("Read failed.\n");
		exit(-1);
	}
	
	printf("%s\n", buffer);
	
	return 0;
}
