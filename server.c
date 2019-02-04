
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, int* argv[])
{
	struct sockaddr_in serverAddr;
	int serverFd;
	
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd <= 0)
	{
		
	}
}
