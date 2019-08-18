#include "sealfile.h"

void saveCiphertext(Ciphertext encrypted, string filename){
  ofstream ct;
  ct.open(filename, ios::binary);
  encrypted.save(ct);
};


Ciphertext loadCiphertext(EncryptionParameters* parms, string filename){
  auto context = SEALContext::Create(*parms);

  ifstream ct;
  ct.open(filename, ios::binary);
  Ciphertext result;
  result.load(context, ct);

  return result;
};


EncryptionParameters loadParametersFromFile(string filename){
  // Save parameters
  ifstream parameters;
  parameters.open(filename, ios::binary);
  return EncryptionParameters::Load(parameters);
}


void savePublicKeyToFile(PublicKey key, string filename){
  ofstream pub;
  pub.open(filename, ios::binary);
  key.save(pub);
};

PublicKey loadPublicKeyFromFile(EncryptionParameters* parms, string filename){

  ifstream pub;
  pub.open(filename, ios::binary);
  PublicKey key;

  auto context = SEALContext::Create(*parms);
  key.load(context, pub);

  return key;
};

RelinKeys loadRelinKeysFromFile(EncryptionParameters* parms, string filename){

  ifstream rel;
  rel.open(filename, ios::binary);
  RelinKeys keys;

  auto context = SEALContext::Create(*parms);
  keys.load(context, rel);

  return keys;
};

bool exists_file (string name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
      fclose(file);
      return true;
    } else {
      return false;
    }
};
