all: client server

client:client.cpp
	@g++ -g -o client client.cpp -lstdc++fs

server:server.cpp
	@g++ -g -o server server.cpp -pthread

clean:
	rm -f client server