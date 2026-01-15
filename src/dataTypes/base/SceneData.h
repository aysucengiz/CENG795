//
// Created by Aysu on 04-Oct-25.
//

#ifndef DATATYPES_H
#define DATATYPES_H

#include <array>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "../../typedefs.h"
#include "Vectors.h"
struct Color{
    real r;
    real g;
    real b;

    Color();
    Color(real r, real g, real b);
    Color(std::string inp);
    Color c_sqrt() const;
    Color& operator+=(const Color& other);
    bool isBlack() const;
    Color operator-();
    Color exponent();
};



struct CVertex
{
    uint32_t id;
    Vertex v;
    Vec3r n;

    CVertex(uint32_t i, Vertex v, Vec3r n);
    CVertex(uint32_t i, const std::string& inp);
};



#endif //DATATYPES_H
