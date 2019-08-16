#ifndef _CURVA_H_INCLUDED_
#define _CURVA_H_INCLUDED_

#include <string>

class Curva {
  public:
    double a, b, c;
    std::string name = "";
    Curva(double a, double b, double c) : a{a}, b{b}, c{c} {
    }

    Curva(std::string name, double a, double b, double c) : name{name}, a{a}, b{b}, c{c} {
    }
};

#endif
