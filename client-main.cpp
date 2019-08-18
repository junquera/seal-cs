#include <iostream>
#include <vector>
#include <string>

#include "client.h"
#include "common/sealfile.h"

using namespace std;

void enc(){
  float temperatura, mes;
  cout << "Inserte temperatura > ";
  cin >> temperatura;
  cout << "Inserte mes > ";
  cin >> mes;

  SClient client;

  Ciphertext x_encrypted, y_encrypted;
  x_encrypted = client.encrypt(mes);
  y_encrypted = client.encrypt(temperatura);

  client.saveConfig();
  saveCiphertext(x_encrypted, "x.data");
  saveCiphertext(y_encrypted, "y.data");
}

void dec(){

  SClient client;
  Ciphertext encrypted_result = loadCiphertext(client.parms, "result.data");

  /*
  Inicializamos el vector de resultados
  */
  vector<string> curvas = loadCurveNames("curve_names.data");

  vector<Distance> result;
  result.reserve(curvas.size());

  vector<double> result_vector = client.decrypt(encrypted_result);
  /*
    Asociamos los resultados del servidor (double's) con los nombres
    de las curvas que los han originado para devolverlos
  */
  for(int i = 0; i < curvas.size(); i++){
    Distance aux_res(curvas[i], result_vector[i]);
    result.push_back(aux_res);
  }

  for(Distance d: result)
    cout << d.name << ": " << d.distance << endl;

}

int main(int argc, char* argv[])
{
  int e;
  cout << "Encrypt(0) or decrypt(1)? > ";
  cin >> e;

  switch (e) {
    case 0:
      enc();
      break;
    case 1:
      dec();
      break;
    default:
      cout << "ERROR: " << e << "?" << endl;
  }

  return 0;
}
