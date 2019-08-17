#ifndef _CLIENT_H_INCLUDED_
#define _CLIENT_H_INCLUDED_

#include <iostream>
#include <fstream>      // std::ofstream


#include "seal/seal.h"
#include "constants.h"
#include <vector>


#include "server.h"
#include "constants.h"

using namespace std;
using namespace seal;


class Distance {
  public:
    string name;
    double distance;

    Distance(string name, double distance) : name{name}, distance{distance} {
    }
};

class SClient {
  public:
    SClient();
    SClient(string config_mask);
    void saveConfig(string config_mask);
    Ciphertext encrypt(double a);
    Ciphertext encrypt(vector<double> a);
    vector<double> decrypt(Ciphertext a);
    void genKeys();
    void setKeysFromFile(string keyFileName);
    PublicKey public_key;
    RelinKeys relin_keys;
    EncryptionParameters* parms;
  private:
    SecretKey secret_key;
    Encryptor* encryptor;
    Evaluator* evaluator;
    Decryptor* decryptor;
    CKKSEncoder* encoder;
    string config_mask;
};

#endif
