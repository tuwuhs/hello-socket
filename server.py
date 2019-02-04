
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('127.0.0.1', 10000))
s.listen(1)
print "Listening..."

conn, addr = s.accept()
print "Accepted connection from", addr

try:
	while True:
		data = conn.recv(1024)
		if not data:
			print "Connection closed by peer"
			break
		print data
		conn.sendall(data)
finally:
	conn.close()
