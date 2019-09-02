SEAL_PATH="../SEAL"
CC=g++
CFLAGS=-Wall -std=c++17

all: main client server test
	echo "Ok!"

test:
	$(CC) $(CFLAGS) test.cpp -L$(SEAL_PATH)/SEAL/native/lib/ -I$(SEAL_PATH)/SEAL/native/src/ -lseal -lpthread -o test

client:
	$(CC) $(CFLAGS) client.cpp  client-main.cpp client.h  constants.h  curva.h  curvas.h common/sealfile.cpp common/sealfile.h -L$(SEAL_PATH)/native/lib/ -I$(SEAL_PATH)/native/src/ -lseal -lpthread -o client

server:
	$(CC) $(CFLAGS) server.cpp server-main.cpp constants.h  curva.h  curvas.h  server.h common/sealfile.cpp common/sealfile.h -L$(SEAL_PATH)/native/lib/ -I$(SEAL_PATH)/native/src/ -lseal -lpthread -o server

clean:
	rm -rf test client server *.data *.key
