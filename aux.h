#ifndef _AUX_H_INCLUDED_
#define _AUX_H_INCLUDED_

#include <iostream>
#include <fstream>      // std::ofstream

#include "seal/seal.h"
#include "constants.h"
#include <vector>

using namespace std;
using namespace seal;

void saveCiphertext(Ciphertext encrypted, string filename);
Ciphertext loadCiphertext(EncryptionParameters* parms, string filename);
void loadParametersFromFile(EncryptionParameters* parms, string filename);
void savePublicKeyToFile(PublicKey key, string filename);
PublicKey loadPublicKeyFromFile(EncryptionParameters* parms, string filename);
bool exists_file (string name);

#endif
