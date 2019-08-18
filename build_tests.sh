g++ *.cpp *.h  common/sealfile.cpp common/sealfile.h -L../SEAL/native/lib/ -lseal -I../SEAL/native/src/ -std=c++17 -lpthread -o main
