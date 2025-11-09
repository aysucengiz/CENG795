//
// Created by vicy on 11/03/25.
//

#ifndef CENG795_MATRIX_H
#define CENG795_MATRIX_H
#include <array>

#include "../base/SceneData.h"
#include <cmath>
#include <ostream>


#define M4T_EPS 1e-7

class M4trix
{
public:
    std::array<std::array<double,4>,4> arr;

    M4trix();
    M4trix(real a);
    M4trix(const std::array<std::array<double,4>,4>& p);
    M4trix(const M4trix& other);
    M4trix& operator=(const M4trix& other);
    M4trix& operator+=(const M4trix& other);
    std::array<double,4>& operator[](int i);
    const std::array<double,4>&operator[](int i) const;

    M4trix Adjugate() const;
    real Determinant() const;
    M4trix Inverse() const;
    M4trix Transpose();


    // M4trix
    friend bool operator==(const M4trix& a, const M4trix& b);
    friend M4trix operator+(const M4trix& a, const M4trix& b);
    friend M4trix operator-(const M4trix& a, const M4trix& b);
    friend M4trix operator*(const M4trix& a, const M4trix& b);
    friend Vec4r operator *(const M4trix &a, const Vec4r &b);
    friend Ray operator *(const M4trix &a, const Ray &b);

};

M4trix Identity();
std::ostream& operator<<(std::ostream& os, const M4trix& a);

#endif //CENG795_MATRIX_H