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



  /*
  Valores de la regresión cuadratica "Cabo de Gata"
  */
  double a = -0.39;
  double b = 5.69;
  double c = 3.97;
  
  cout << "Evaluating polynomial " << a << "*x^2 + " << b << "x + " << c << " ..." << endl;

  Plaintext a_plain, b_plain, c_plain;
  encoder.encode(a, scale, a_plain);
  encoder.encode(b, scale, b_plain);
  encoder.encode(c, scale, c_plain);


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
  Ciphertext p1_encrypted, p2_encrypted, px_encrypted, p3_encrypted, p_encrypted, encrypted_result;

  // TODO Ver cuando tengo que reescalar
  evaluator.multiply_plain(x_encrypted, a_plain, p1_encrypted);
  evaluator.relinearize_inplace(p1_encrypted, relin_keys);
  // Reescalado tras multiplicación
  evaluator.rescale_to_next_inplace(p1_encrypted);

  // Quitamos uno a chain de x para poder multiplicar x_encry por p1_encrypted
  evaluator.mod_switch_to_next(x_encrypted, px_encrypted);
  evaluator.multiply(px_encrypted, p1_encrypted, p2_encrypted);
  evaluator.rescale_to_next_inplace(p2_encrypted);

  evaluator.multiply_plain(x_encrypted, b_plain, p3_encrypted);
  evaluator.rescale_to_next_inplace(p3_encrypted);
  evaluator.mod_switch_to_next_inplace(p3_encrypted);


  p2_encrypted.scale() = scale;
  p3_encrypted.scale() = scale;

  evaluator.add(p2_encrypted, p3_encrypted, p_encrypted);

  // Hemos bajado 2 posiciones en la cadena
  evaluator.mod_switch_to_next_inplace(c_plain);
  evaluator.mod_switch_to_next_inplace(c_plain);
  evaluator.add_plain_inplace(p_encrypted, c_plain);

  // Hemos bajado 2 posiciones en la cadena
  evaluator.mod_switch_to_next_inplace(y_encrypted);
  evaluator.mod_switch_to_next_inplace(y_encrypted);
  evaluator.sub(y_encrypted, p_encrypted, encrypted_result);


  /*
  First print the true result.
  */
  Plaintext plain_result;
  decryptor.decrypt(encrypted_result, plain_result);
  vector<double> result_vector;
  encoder.decode(plain_result, result_vector);
  double result = result_vector[0];
  cout << "Result: " << result << endl;


  return 0;
}
