#include "server.h"
#include "common/sealfile.h"
#include "curvas.h"

int main() {
  SServer server;
  server.addCurva(curva_cabo_de_gata);
  server.addCurva(curva_finisterre);

  cout << "curves" << endl;

  EncryptionParameters parms = loadParametersFromFile("params.data");
  RelinKeys rel = loadRelinKeysFromFile(&parms, "relin.key");


  cout << "rel" << endl;

  Ciphertext x_encrypted = loadCiphertext(&parms, "x.data");
  Ciphertext y_encrypted = loadCiphertext(&parms, "y.data");


  cout << "texts" << endl;

  Ciphertext encrypted_result = server.distance(x_encrypted, y_encrypted, rel);
  saveCiphertext(encrypted_result, "result.data");
  saveCurveNames(server.getCurveNames(), "curve_names.data");

}
