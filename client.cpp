#include "client.h"
#include "aux.h"

void SClient::genKeys() {
	// generaClaves(key);
};

void SClient::setKeysFromFile(string keyFileName){
	// loadSecretKeyFromFile(keyFileName, key);
};


void SClient::saveConfig(string config_mask){

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

  // Save parameters
  ofstream parameters, pub, sec, rel;
  parameters.open(params_path, ios::binary);
  EncryptionParameters::Save(*parms, parameters);

  pub.open(pub_key_path, ios::binary);
  sec.open(sec_key_path, ios::binary);
  rel.open(rel_key_path, ios::binary);

  public_key.save(pub);
  secret_key.save(sec);
  relin_keys.save(rel);
};

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
