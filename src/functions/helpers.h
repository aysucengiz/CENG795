//
// Created by vicy on 10/18/25.
//

#ifndef CENG795_HELPERS_H
#define CENG795_HELPERS_H

#include "../dataTypes/base/Vectors.h"
#include "../dataTypes/base/SceneData.h"

int clamp(real c, int from, int to);
real max3(real a, real b, real c);
real min3(real a, real b, real c);
Vertex maxVert3(Vertex a, Vertex b, Vertex c);
Vertex minVert3(Vertex a, Vertex b, Vertex c);
Vertex maxVert2(Vertex a, Vertex b);
Vertex minVert2(Vertex a, Vertex b);
real dot_product(const Vec3r &a, const Vec3r &b);
real determinant(const Vec3r &first, const Vec3r &second, const Vec3r &third );
Axes next(Axes a);
real G(std::array<real,2> locs, real &inv_stddev_2);
real InvStdDev(Color &mean, std::vector<Color> &colors);
Color Mean(std::vector<Color> &colors);
real getRandom();
real getRandomTime();
std::pair<int,int> closestFactors(int n);
#endif //CENG795_HELPERS_H