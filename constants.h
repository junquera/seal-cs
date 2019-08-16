#ifndef _CONSTANTS_H_INCLUDED_
#define _CONSTANTS_H_INCLUDED_

#include <cmath>
#include "curva.h"

const double SCALE = pow(2.0, 40);


/*
  Valores de la regresión cuadratica "Cabo de Gata"
*/
const Curva curva_cabo_de_gata("cabo_de_gata", -0.41, 5.92, 2.43);

/*
  Valores de la regresión cuadratica "Finisterre"
*/
const Curva curva_finisterre("finisterre", -0.25, 3.78, 4.38);

#endif
