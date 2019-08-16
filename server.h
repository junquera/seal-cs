#ifndef _SERVER_H_INCLUDED_
#define _SERVER_H_INCLUDED_

#include <iostream>
#include <fstream>      // std::ofstream

#include "seal/seal.h"
#include <vector>

#include "constants.h"
#include "curva.h"

using namespace std;
using namespace seal;


class SServer {
  public:
    SServer();
    Ciphertext distance(vector<Curva> curvas, Ciphertext x_encrypted, Ciphertext y_encrypted, RelinKeys relin_keys);
  private:
    Evaluator* evaluator;
    CKKSEncoder* encoder;
};

#endif
