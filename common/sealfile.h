#ifndef _SEALFILE_H_INCLUDED_
#define _SEALFILE_H_INCLUDED_

#include <iostream>
#include <fstream>      // std::ofstream

#include "seal/seal.h"
#include <vector>

using namespace std;
using namespace seal;

// TODO Poner namespace?
bool exists_file (string name);
void saveCiphertext(Ciphertext encrypted, string filename);
Ciphertext loadCiphertext(EncryptionParameters* parms, string filename);
EncryptionParameters loadParametersFromFile(string filename);
void savePublicKeyToFile(PublicKey key, string filename);
PublicKey loadPublicKeyFromFile(EncryptionParameters* parms, string filename);
RelinKeys loadRelinKeysFromFile(EncryptionParameters* parms, string filename);
#endif
