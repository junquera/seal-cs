#include "client.h"

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

bool exists_file (string name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
      fclose(file);
      return true;
    } else {
      return false;
    }
};

void SClient::genKeys() {
	// generaClaves(key);
};

void SClient::setKeysFromFile(string keyFileName){
	// loadSecretKeyFromFile(keyFileName, key);
};

void loadParametersFromFile(EncryptionParameters* parms, string filename){
  // Save parameters
  ifstream parameters;
  parameters.open(filename, ios::binary);
  EncryptionParameters _parms = EncryptionParameters::Load(parameters);
  parms = &_parms;
}

SClient::SClient(){
  SClient("");
};

SClient::SClient(string config_mask) {

  stringstream stringStream;

  stringStream.str("");
  stringStream << config_mask << "params.data";
  string params_path = stringStream.str();

  stringStream.str("");
  stringStream << config_mask << "public.key";
  string pub_key_path = stringStream.str();

  stringStream.str("");
  stringStream << config_mask << "secret.key";
  string sec_key_path = stringStream.str();

  stringStream.str("");
  stringStream << config_mask << "relin.key";
  string rel_key_path = stringStream.str();

  if(exists_file(params_path)){
    loadParametersFromFile(parms, params_path);
  } else {
    parms = new EncryptionParameters(scheme_type::CKKS);
    size_t poly_modulus_degree = 8192;
    parms->set_poly_modulus_degree(poly_modulus_degree);
    parms->set_coeff_modulus(CoeffModulus::Create(
    poly_modulus_degree, { 60, 40, 40, 60 }));

    // Save parameters
    ofstream parameters;
    parameters.open(params_path, ios::binary);
    EncryptionParameters::Save(*parms, parameters);
  }


  auto context = SEALContext::Create(*parms);
  // print_parameters(context);
  // cout << endl;

  if( exists_file(pub_key_path) &&
      exists_file(sec_key_path) &&
      exists_file(rel_key_path)) {

    ifstream pub, sec, rel;
    pub.open(pub_key_path, ios::binary);
    sec.open(sec_key_path, ios::binary);
    rel.open(rel_key_path, ios::binary);

    public_key.load(context, pub);
    secret_key.load(context, sec);
    relin_keys.load(context, rel);

  } else {
    KeyGenerator keygen(context);
    public_key = keygen.public_key();
    secret_key = keygen.secret_key();
    relin_keys = keygen.relin_keys();

    // Save keys
    ofstream pub, sec, rel;
    pub.open(pub_key_path, ios::binary);
    sec.open(sec_key_path, ios::binary);
    rel.open(rel_key_path, ios::binary);

    public_key.save(pub);
    secret_key.save(sec);
    relin_keys.save(rel);
  }

  encryptor = new Encryptor(context, public_key);
  evaluator = new Evaluator(context);
  decryptor = new Decryptor(context, secret_key);
  encoder = new CKKSEncoder(context);

};

Ciphertext SClient::encrypt(double a) {

  Plaintext plain;
  encoder->encode(a, SCALE, plain);

  Ciphertext encrypted;
  encryptor->encrypt(plain, encrypted);

  return encrypted;
};


Ciphertext SClient::encrypt(vector<double> a) {

  size_t slot_count = encoder->slot_count();
  if(a.size() > slot_count){
    // Si no se pueden codificar tantas curvas
    throw "Error: No hay suficiente espacio para tantos valores";
  }

  Plaintext plain;
  encoder->encode(a, SCALE, plain);

  Ciphertext encrypted;
  encryptor->encrypt(plain, encrypted);

  return encrypted;
};

vector<double> SClient::decrypt(Ciphertext a) {

  Plaintext plain_result;
  decryptor->decrypt(a, plain_result);

  vector<double> result_vector;
  encoder->decode(plain_result, result_vector);

  return result_vector;
};

vector<Distance> distance(SClient client, double mes, double temperatura){

  Ciphertext x_encrypted, y_encrypted, encrypted_result;
  x_encrypted = client.encrypt(mes);
  y_encrypted = client.encrypt(temperatura);

  /*
  Inicializamos las curvas con las que queremos trabajar
  */
  SServer server;
  server.addCurva(curva_cabo_de_gata);
  server.addCurva(curva_finisterre);

  /*
  Inicializamos el vector de resultados
  */
  vector<string> curvas = server.getCurveNames();
  vector<Distance> result;
  result.reserve(curvas.size());
  encrypted_result = server.distance(x_encrypted, y_encrypted, client.relin_keys);

  vector<double> result_vector = client.decrypt(encrypted_result);

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

int main(int argc, char** argv) {

  /*
  Temperatura en Junio de 2018 en Cabo de Gata
  */
  int x = 6;
  float y = 23.0;

  SClient client;

  vector<Distance> result = distance(client, x, y);
  for(int i = 0; i < result.size(); i++)
    cout << result[i].name << ": " << result[i].distance << endl;

  Ciphertext test_x = client.encrypt(x);
  saveCiphertext(test_x, "x.data");
  Ciphertext test_x2 = loadCiphertext(client.parms, "x.data");
  cout << client.decrypt(test_x2)[1] << endl;


  return 0;
}
