all: main client server test
	echo "Ok!"

test:
	g++ -std=c++17 test.cpp -L../SEAL/native/lib/ -I../SEAL/native/src/ -lseal -lpthread -o test

client:
	g++ -std=c++17 client.cpp  client-main.cpp client.h  constants.h  curva.h  curvas.h common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -I../SEAL/native/src/ -lseal -lpthread -o client

server:
	g++ -std=c++17 server.cpp server-main.cpp constants.h  curva.h  curvas.h  server.h common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -I../SEAL/native/src/ -lseal -lpthread -o server

clean:
	rm -rf test client server *.data *.key
