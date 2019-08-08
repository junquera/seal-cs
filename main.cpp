#include <iostream>


#include "seal/seal.h"
#include "main.h"
#include <vector>


using namespace std;
using namespace seal;

int main() {

  EncryptionParameters parms(scheme_type::CKKS);


  size_t poly_modulus_degree = 8192;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::Create(
      poly_modulus_degree, { 60, 40, 40, 60 }));

  double scale = pow(2.0, 40);

  auto context = SEALContext::Create(parms);
  // print_parameters(context);
  // cout << endl;

  KeyGenerator keygen(context);
  auto public_key = keygen.public_key();
  auto secret_key = keygen.secret_key();
  auto relin_keys = keygen.relin_keys();

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  CKKSEncoder encoder(context);
  size_t slot_count = encoder.slot_count();
  cout << "Number of slots: " << slot_count << endl;

  // vector<double> input;
  // input.reserve(slot_count);
  // double curr_point = 0;
  // double step_size = 1.0 / (static_cast<double>(slot_count) - 1);
  // for (size_t i = 0; i < slot_count; i++, curr_point += step_size)
  // {
  //     input.push_back(curr_point);
  // }
  // cout << "Input vector: " << endl;
  // print_vector(input, 3, 7);



  cout << "Evaluating polynomial PI*x^3 + 0.4x + 1 ..." << endl;

  /*
  Valores de la regresión cuadratica "Cabo de Gata"
  */
  double a1 = -0.39;
  double a2 = 5.69;
  double a3 = 3.97;

  Plaintext a1_plain, a2_plain, a3_plain;
  encoder.encode(a1, scale, a1_plain);
  encoder.encode(a2, scale, a2_plain);
  encoder.encode(a3, scale, a3_plain);


  /*
  Temperatura en Junio de 2018 en Cabo de Gata
  */
  int x = 6;
  float y = 23.0;

  Plaintext x_plain, y_plain;

  encoder.encode(x, scale, x_plain);
  encoder.encode(y, scale, y_plain);

  Ciphertext x_encrypted, y_encrypted;
  encryptor.encrypt(x_plain, x_encrypted);
  encryptor.encrypt(y_plain, y_encrypted);

  /*
    Calcularemos:

    p = a1*x^2 + a2*x + a3
    resultado = y - p

    Para ello:

    p1 = x^2
    p2 = a1*p1
    p3 = a2*x
    p = p1 + p2
    p = p + a3
    resultado = y - p

  */
  Ciphertext p1_encrypted, p2_encrypted, p3_encrypted, p_encrypted, encrypted_result;

  // TODO Ver cuando tengo que reescalar
  evaluator.square(x_encrypted, p1_encrypted);
  evaluator.relinearize_inplace(p1_encrypted, relin_keys);

  evaluator.multiply_plain(p1_encrypted, a1_plain, p2_encrypted);
  evaluator.multiply_plain(x_encrypted, a2_plain, p3_encrypted);

  evaluator.add(p1_encrypted, p2_encrypted, p_encrypted);
  evaluator.add_plain_inplace(p_encrypted, a3_plain);

  evaluator.sub(y_encrypted, p_encrypted, encrypted_result);


  /*
  First print the true result.
  */
  Plaintext plain_result;
  decryptor.decrypt(encrypted_result, plain_result);
  vector<double> result_vector;
  encoder.decode(plain_result, result_vector);
  double result = result_vector[0];
  cout << "Result:" << result << endl;


  return 0;
}
