#include "server.h"

SServer::SServer() : SServer("") {
};

SServer::SServer(string config_mask) {

  stringstream stringStream;

  stringStream.str("");
  stringStream << config_mask << "params.data";
  string params_path = stringStream.str();

  EncryptionParameters parms = loadParametersFromFile(params_path);

  auto context = SEALContext::Create(parms);

  evaluator = new Evaluator(context);
  encoder = new CKKSEncoder(context);
};

void SServer::addCurva(Curva c){
  curvas.push_back(c);
};

Ciphertext SServer::distance(Ciphertext x_encrypted, Ciphertext y_encrypted, RelinKeys relin_keys){

    /*
     TODO Hacer vectores con las temperaturas
     // Como en el ejemplo 5, pero con una fila por cada ciudad,
     Mejor aún, un vector para las aes, otro para las bs y otro para las cs
    */
    Plaintext a_plain, b_plain, c_plain;

    size_t slot_count = encoder->slot_count();
    if(curvas.size() > slot_count){
      // Si no se pueden codificar tantas curvas
      throw "Error: No hay suficiente espacio para tantas curvas";
    }

    vector<double> a, b, c;
    for(int i = 0; i < curvas.size(); i++){
      a.push_back(curvas[i].a);
      b.push_back(curvas[i].b);
      c.push_back(curvas[i].c);
    }

    encoder->encode(a, SCALE, a_plain);
    encoder->encode(b, SCALE, b_plain);
    encoder->encode(c, SCALE, c_plain);

    // cout << "Evaluating polynomial " << a << "*x^2 + " << b << "x + " << c << " ..." << endl;

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
    evaluator->multiply_plain(x_encrypted, a_plain, p1_encrypted);
    evaluator->relinearize_inplace(p1_encrypted, relin_keys);
    // Reescalado tras multiplicación
    evaluator->rescale_to_next_inplace(p1_encrypted);

    // Quitamos uno a chain de x para poder multiplicar x_encry por p1_encrypted
    evaluator->mod_switch_to_next(x_encrypted, px_encrypted);
    evaluator->multiply(px_encrypted, p1_encrypted, p2_encrypted);
    evaluator->rescale_to_next_inplace(p2_encrypted);

    evaluator->multiply_plain(x_encrypted, b_plain, p3_encrypted);
    evaluator->rescale_to_next_inplace(p3_encrypted);
    evaluator->mod_switch_to_next_inplace(p3_encrypted);

    p2_encrypted.scale() = SCALE;
    p3_encrypted.scale() = SCALE;

    evaluator->add(p2_encrypted, p3_encrypted, p_encrypted);

    // Hemos bajado 2 posiciones en la cadena
    evaluator->mod_switch_to_next_inplace(c_plain);
    evaluator->mod_switch_to_next_inplace(c_plain);
    evaluator->add_plain_inplace(p_encrypted, c_plain);

    // Hemos bajado 2 posiciones en la cadena
    evaluator->mod_switch_to_next_inplace(y_encrypted);
    evaluator->mod_switch_to_next_inplace(y_encrypted);
    evaluator->sub(y_encrypted, p_encrypted, encrypted_result);

    return encrypted_result;

};

vector<string> SServer::getCurveNames(){
  vector<string> names;
  names.reserve(curvas.size());
  for(Curva c: curvas)
    names.push_back(c.name);

  return names;
};
