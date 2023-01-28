server: server.cpp hashtable.cpp
	g++ -std=c++17 -o server server.cpp hashtable.cpp

client: client.cpp
	g++ -std=c++17 -o client client.cpp

clean:
	rm -rf server
	rm -rf client