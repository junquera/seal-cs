#include "server.h"

SServer::SServer(){
}

Ciphertext SServer::distance(Ciphertext x_encrypted, Ciphertext y_encrypted){

    /*
    Valores de la regresión cuadratica "Cabo de Gata"
    */
    double a = -0.39;
    double b = 5.69;
    double c = 3.97;

    Plaintext a_plain, b_plain, c_plain;
    encoder.encode(a, scale, a_plain);
    encoder.encode(b, scale, b_plain);
    encoder.encode(c, scale, c_plain);

    cout << "Evaluating polynomial " << a << "*x^2 + " << b << "x + " << c << " ..." << endl;
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

    return encrypted_result;

}
