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


class SClient {
  public:
    SClient();
    double distance(double mes, double temperatura);

  private:
    PublicKey public_key;
    SecretKey secret_key;
    RelinKeys relin_keys;
    Encryptor encryptor;
    Evaluator evaluator;
    Decryptor decryptor;
    CKKSEncoder encoder;
};

#endif
