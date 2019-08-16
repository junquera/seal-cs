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
  EncryptionParameters::Save(parms, parameters);

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

  encryptor = new Encryptor(context, public_key);
  evaluator = new Evaluator(context);
  decryptor = new Decryptor(context, secret_key);
  encoder = new CKKSEncoder(context);

}

vector<Distance> SClient::distance(double mes, double temperatura){

  Plaintext x_plain, y_plain;
  encoder->encode(mes, SCALE, x_plain);
  encoder->encode(temperatura, SCALE, y_plain);

  Ciphertext x_encrypted, y_encrypted, encrypted_result;
  encryptor->encrypt(x_plain, x_encrypted);
  encryptor->encrypt(y_plain, y_encrypted);


  /*
  Inicializamos las curvas con las que queremos trabajar
  */
  SServer server;
  server.addCurva(curva_cabo_de_gata);
  server.addCurva(curva_finisterre);

  /*
  Inicializamos el vector de resultados
  */
  vector<Distance> result;
  vector<string> curvas = server.getCurveNames();
  result.reserve(curvas.size());

  encrypted_result = server.distance(x_encrypted, y_encrypted, relin_keys);

  Plaintext plain_result;
  decryptor->decrypt(encrypted_result, plain_result);

  vector<double> result_vector;
  encoder->decode(plain_result, result_vector);

  /*
    Asociamos los resultados del servidor (double's) con los nombres
    de las curvas que los han originado para devolverlos
  */
  for(int i = 0; i < curvas.size(); i++){
    Distance aux_res(curvas[i], result_vector[i]);
    result.push_back(aux_res);
  }

  return result;

}

int main() {

  /*
  Temperatura en Junio de 2018 en Cabo de Gata
  */
  int x = 6;
  float y = 23.0;

  SClient client;
  vector<Distance> result = client.distance(x, y);
  for(int i = 0; i < result.size(); i++)
    cout << result[i].name << ": " << result[i].distance << endl;


  return 0;
}
