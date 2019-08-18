# g++ *.cpp *.h  common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -lseal -I../SEAL/native/src/ -std=c++17 -lpthread -o main
# g++ client.cpp  main.cpp  server.cpp server-main.cpp client.h constants.h  curva.h  curvas.h  server.h common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -lseal -I../SEAL/native/src/ -std=c++17 -lpthread -o main
g++ client.cpp  client-main.cpp client.h  constants.h  curva.h  curvas.h common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -lseal -I../SEAL/native/src/ -std=c++17 -lpthread -o client
g++ server.cpp server-main.cpp constants.h  curva.h  curvas.h  server.h common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -lseal -I../SEAL/native/src/ -std=c++17 -lpthread -o server
