#include "client.h"
#include "common/sealfile.h"
#include "curvas.h"

vector<Distance> distance(SClient client, SServer server, double mes, double temperatura){

  Ciphertext x_encrypted, y_encrypted, encrypted_result;
  x_encrypted = client.encrypt(mes);
  y_encrypted = client.encrypt(temperatura);

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

int main(int argc, char** argv) {

  /*
  Temperatura en Junio de 2018 en Cabo de Gata
  */
  int x = 6;
  float y = 23.0;

  SClient client;
  // cout << client.parms->poly_modulus_degree() <<  endl;
  client.saveConfig();

  /*
  Inicializamos las curvas con las que queremos trabajar
  */
  SServer server;
  server.addCurva(curva_cabo_de_gata);
  server.addCurva(curva_finisterre);


  vector<Distance> result = distance(client, server, x, y);
  for(int i = 0; i < result.size(); i++)
    cout << result[i].name << ": " << result[i].distance << endl;

  // Ciphertext test_x = client.encrypt(x);
  // saveCiphertext(test_x, "x.data");
  // Ciphertext test_x2 = loadCiphertext(client.parms, "x.data");
  // cout << client.decrypt(test_x2)[1] << endl;


  return 0;
}
