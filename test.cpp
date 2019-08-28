
#include <iostream>
#include <fstream>      // std::ofstream


#include "seal/seal.h"
#include "constants.h"
#include "common/sealfile.h"

#include <vector>

#include <cmath>
#include "server.h"
#include "constants.h"

using namespace std;

void test_n_products(int poly_modulus_degree_bits){

  size_t poly_modulus_degree = pow(2, poly_modulus_degree_bits);
  auto coeff_modulus = CoeffModulus::BFVDefault(poly_modulus_degree);

  EncryptionParameters parms(scheme_type::BFV);
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(coeff_modulus);
  parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, min(20*(poly_modulus_degree_bits - 9), 60)));

  auto context = SEALContext::Create(parms);

  KeyGenerator keygen(context);
  auto public_key = keygen.public_key();
  auto secret_key = keygen.secret_key();
  auto relin_keys = keygen.relin_keys();

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  BatchEncoder encoder(context);
  size_t slot_count = encoder.slot_count();

  Plaintext a_plain, b_plain;
  vector<long> a, b;
  for(int i = 1; i < 15; i++){
    a.push_back(i);
    b.push_back(15 - i);
  }

  encoder.encode(a, a_plain);
  encoder.encode(b, b_plain);

  Ciphertext a_encrypted, b_encrypted;
  encryptor.encrypt(a_plain, a_encrypted);
  encryptor.encrypt(b_plain, b_encrypted);

  int noise_budget = decryptor.invariant_noise_budget(a_encrypted);

  int res = 0;
  while(noise_budget > 0){
    res++;
    evaluator.multiply_inplace(a_encrypted, b_encrypted);
    // evaluator.relinearize_inplace(a_encrypted, relin_keys);
    noise_budget = decryptor.invariant_noise_budget(a_encrypted);
  }

  cout << poly_modulus_degree_bits << "," << res << endl;

};


void test_times(int poly_modulus_degree_bits){

  size_t poly_modulus_degree = pow(2, poly_modulus_degree_bits);

  if(CoeffModulus::MaxBitCount(poly_modulus_degree) < 120)
    return;

  int coeff_modulus_max_len = CoeffModulus::MaxBitCount(poly_modulus_degree);
  vector<int> coeff_modulus;
  int values = ((coeff_modulus_max_len-120)/40);
  coeff_modulus.reserve(values);

  coeff_modulus.push_back(60);

  for(int i = 0; i < values-2; i++)
    coeff_modulus.push_back(40);

  coeff_modulus.push_back(60);

  EncryptionParameters parms(scheme_type::CKKS);
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(
    CoeffModulus::Create(
      poly_modulus_degree, coeff_modulus
    )
  );

  auto context = SEALContext::Create(parms);

  KeyGenerator keygen(context);
  auto public_key = keygen.public_key();
  auto secret_key = keygen.secret_key();
  auto relin_keys = keygen.relin_keys();

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  CKKSEncoder encoder(context);
  size_t slot_count = encoder.slot_count();

  Plaintext a_plain, b_plain;

  vector<double> input;
  input.reserve(slot_count);

  double curr_point = 0;
  double step_size = 1.0 / (static_cast<double>(slot_count) - 1);
  for (size_t i = 0; i < slot_count; i++, curr_point += step_size)
  {
    input.push_back(curr_point);
  }
  cout << "slots with " << poly_modulus_degree << " poly_modulus_degree: " << slot_count << endl;

  double scale = pow(2.0, 20*(values-2));

  time_t t0 = time(NULL);
  encoder.encode(input, scale, a_plain);
  cout << "encode_batch," << poly_modulus_degree_bits << "," << time(NULL) - t0 << endl;

  t0 = time(NULL);
  encoder.encode(3.14, scale, b_plain);
  cout << "encode," << poly_modulus_degree_bits << "," << time(NULL) - t0 << endl;


  Ciphertext a_encrypted, b_encrypted;

  t0 = time(NULL);
  encryptor.encrypt(a_plain, a_encrypted);
  cout << "encrypt_batch," << poly_modulus_degree_bits << "," << time(NULL) - t0 << endl;

  t0 = time(NULL);
  encryptor.encrypt(b_plain, b_encrypted);
  cout << "encrypt_number," << poly_modulus_degree_bits << "," << time(NULL) - t0 << endl;

  t0 = time(NULL);
  evaluator.multiply_inplace(a_encrypted, b_encrypted);
  cout << "multiply," << poly_modulus_degree_bits << "," << time(NULL) - t0 << endl;

  t0 = time(NULL);
  evaluator.add_inplace(b_encrypted, b_encrypted);
  cout << "add," << poly_modulus_degree_bits << "," << time(NULL) - t0 << endl;

};

void test() {
  size_t poly_modulus_degree = 8192;

  if(CoeffModulus::MaxBitCount(poly_modulus_degree) < 120)
    return;

  int coeff_modulus_max_len = CoeffModulus::MaxBitCount(poly_modulus_degree);
  vector<int> coeff_modulus;
  int values = ((coeff_modulus_max_len-120)/40);
  coeff_modulus.reserve(values);

  coeff_modulus.push_back(60);

  for(int i = 0; i < values-2; i++)
    coeff_modulus.push_back(40);

  coeff_modulus.push_back(60);

  EncryptionParameters parms(scheme_type::CKKS);
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(
    CoeffModulus::Create(
      poly_modulus_degree, coeff_modulus
    )
  );

  auto context = SEALContext::Create(parms);

  KeyGenerator keygen(context);
  auto public_key = keygen.public_key();
  auto secret_key = keygen.secret_key();
  auto relin_keys = keygen.relin_keys();

  cout << 0 << endl;
}

int main(int argc, char* argv[])
{
  /*

  El primer y el último número de la cadena tienen que ser mayores que el número a cifrar/descifrar, y los intermedios tienen que ser lo algo más grandes  que los intermedios para asegurar la precisión, y la suma de estos dos con los intermedios tiene que ser menos que max coeff_modulus bit-length. POr lo tanto:

  - Para un número de 40 bits, es necesario utilizr al menos poly_modulus_degree de 8192, y se pueden hacer 4 operaciones.
  - Para un número de 64 bits, es necesario utilizr al menos poly_modulus_degree de 16384, y se pueden hacer 7 operaciones.
  - Para un número de 64 bits, es necesario utilizr al menos poly_modulus_degree de 16384, y se pueden hacer 14 operaciones.

    +----------------------------------------------------+
    | poly_modulus_degree | max coeff_modulus bit-length |
    +---------------------+------------------------------+
    | 1024                | 27                           |
    | 2048                | 54                           |
    | 4096                | 109                          |
    | 8192                | 218                          |
    | 16384               | 438                          |
    | 32768               | 881                          |
    +---------------------+------------------------------+
  */

  // test();
  for(int i = 0; i <= 5; i++){
    cout << "poly_modulus_degree_bits,n" << endl;
    test_n_products(10 + i);
    // cout << "op,poly_modulus_degree_bits,t" << endl;
    // test_times(10 + i);
  }

}
