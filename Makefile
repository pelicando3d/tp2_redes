COMPILER = gcc
OPTIONS = -c -g -Wall

all: client server

server: tp_socket.o janela.o io.o time.o server.o
	$(COMPILER) tp_socket.o janela.o io.o time.o server.o -o server

client: tp_socket.o janela.o io.o time.o client.o
	$(COMPILER) tp_socket.o janela.o io.o time.o client.o -o client

servidor.o: server.c
	$(COMPILER) $(OPTIONS) server.c

client.o: client.c
	$(COMPILER) $(OPTIONS) client.c

io.o: io.c
	$(COMPILER) $(OPTIONS) io.c

time.o: time.c
	$(COMPILER) $(OPTIONS) time.c

janela.o: janela.c
	$(COMPILER) $(OPTIONS) janela.c

tp_socket.o: tp_socket.c
	$(COMPILER) $(OPTIONS) tp_socket.c

clean:
	rm -f *.o client server janela io time tp_socket