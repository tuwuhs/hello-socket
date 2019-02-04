
.PHONY: all
all: client server
	

client: client.c

server: server.c

.PHONY: clean
clean:
	rm -f client server
