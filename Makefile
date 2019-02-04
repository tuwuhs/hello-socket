
.PHONY: all
all: client server
	

client: client.c
	$(CC) $^ -o $@

server:
	

.PHONY: clean
clean:
	rm -f client server
