#include "client.h"

SClient::SClient(){

  EncryptionParameters parms(scheme_type::CKKS);

  size_t poly_modulus_degree = 8192;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::Create(
      poly_modulus_degree, { 60, 40, 40, 60 }));

  // Save parameters
  ofstream parameters;
  parameters.open("parameters.data", ios::binary);
  EncryptionParameters.Save(parms, parameters);

  auto context = SEALContext::Create(parms);
  // print_parameters(context);
  // cout << endl;

  KeyGenerator keygen(context);
  public_key = keygen.public_key();
  secret_key = keygen.secret_key();
  relin_keys = keygen.relin_keys();


  // Save keys
  ofstream pub, sec, rel;
  pub.open("public.key", ios::binary);
  sec.open("secret.key", ios::binary);
  rel.open("relin.key", ios::binary);

  public_key.save(pub);
  secret_key.save(sec);
  relin_keys.save(rel);

  encryptor = Encryptor(context, public_key);
  evaluator = Evaluator(context);
  decryptor = Decryptor(context, secret_key);
  encoder = CKKSEncoder(context);

}

double SClient::distancia(double mes, double temperatura){

  Plaintext x_plain, y_plain;

  encoder.encode(mes, scale, x_plain);
  encoder.encode(temperatura, scale, y_plain);

  Ciphertext x_encrypted, y_encrypted, encrypted_result;
  encryptor.encrypt(x_plain, x_encrypted);
  encryptor.encrypt(y_plain, y_encrypted);

  SServer server;
  encrypted_result = server.distance(x_encrypted, y_encrypted);

  Plaintext plain_result;
  decryptor.decrypt(encrypted_result, plain_result);

  vector<double> result_vector;
  encoder.decode(plain_result, result_vector);

  return result_vector[0];

}

int main(Ciphertext encrypted_result) {

  /*
  Temperatura en Junio de 2018 en Cabo de Gata
  */
  int x = 6;
  float y = 23.0;

  SClient client;
  double result = client.distance(x, y);
  cout << "Result: " << result << endl;


  return 0;
}
