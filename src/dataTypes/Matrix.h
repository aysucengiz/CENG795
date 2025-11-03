//
// Created by vicy on 11/03/25.
//

#ifndef CENG795_MATRIX_H
#define CENG795_MATRIX_H
#include "DataTypes.h"
#include <cmath>
#include <ostream>


#define M4T_EPS 1e-7

class M4trix
{
public:
    real ptr[4][4];

    M4trix();
    M4trix(real a);
    M4trix(const M4trix& other);
    M4trix& operator=(const M4trix& other);
    M4trix& operator+=(const M4trix& other);
    real* operator[](int i) { return ptr[i]; }
    const real* operator[](int i) const { return ptr[i]; }
};

M4trix Identity();
bool operator==(const M4trix& a, const M4trix& b);
M4trix operator+(const M4trix& a, const M4trix& b);
M4trix operator-(const M4trix& a, const M4trix& b);
M4trix operator*(const M4trix& a, const M4trix& b);
std::ostream& operator<<(std::ostream& os, const M4trix& a);


#endif //CENG795_MATRIX_H