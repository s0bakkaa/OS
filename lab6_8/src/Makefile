files: client server

client: client.cpp
	g++ -fsanitize=address client.cpp -lzmq -o client -w

server: server.cpp
	g++ -fsanitize=address server.cpp -lzmq -o server -w

clean:
	rm  -rf client  server
