all: main client server
	echo "Ok!"

main:
	g++ -std=c++17 client.cpp  main.cpp  server.cpp server-main.cpp client.h constants.h  curva.h  curvas.h  server.h common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -I../SEAL/native/src/ -lseal -lpthread -o main

client:
	g++ -std=c++17 client.cpp  client-main.cpp client.h  constants.h  curva.h  curvas.h common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -I../SEAL/native/src/ -lseal -lpthread -o client

server:
	g++ -std=c++17 server.cpp server-main.cpp constants.h  curva.h  curvas.h  server.h common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -I../SEAL/native/src/ -lseal -lpthread -o server

clean:
	rm -rf main client server *.data *.key
