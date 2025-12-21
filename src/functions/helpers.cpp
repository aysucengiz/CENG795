//
// Created by vicy on 10/18/25.
//

#include "helpers.h"

#include <random>

#include "overloads.h"
#include "../dataTypes/object/Object.h"
#include "../dataTypes/object/Mesh.h"


real max3(real a, real b, real c)
{
    return std::max(a, std::max(b, c));
}

real min3(real a, real b, real c)
{
    return std::min(a, std::min(b, c));
}


Vertex maxVert3(Vertex a, Vertex b, Vertex c)
{
    return Vertex(
        max3(a.x, b.x, c.x),
        max3(a.y, b.y, c.y),
        max3(a.z, b.z, c.z)
    );
}

Vertex minVert3(Vertex a, Vertex b, Vertex c)
{
    return Vertex(
        min3(a.x, b.x, c.x),
        min3(a.y, b.y, c.y),
        min3(a.z, b.z, c.z)
    );
}


Vertex maxVert2(Vertex a, Vertex b)
{
    return Vertex(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

Vertex minVert2(Vertex a, Vertex b)
{
    return Vertex(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

int clamp(const real c, const int from, const int to)
{
    int temp = static_cast<int>(std::round(c));
    if (temp > to) return to;
    if (temp < from) return from;
    return temp;
}

Vec3r x_product(const Vec3r& v, const Vec3r& w)
{
    return Vec3r(v.j * w.k - v.k * w.j,
                 v.k * w.i - v.i * w.k,
                 v.i * w.j - v.j * w.i);
}

real dot_product(const Vec3r& a, const Vec3r& b)
{
    return a.i * b.i + a.j * b.j + a.k * b.k;
}

real determinant(const Vec3r& first, const Vec3r& second, const Vec3r& third)
{
    //  a  d  g
    //  b  e  h
    //  c  f  i
    // return a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h -e*g);
    return first.i * (second.j * third.k - third.j * second.k) + first.j * (third.i * second.k - second.i * third.k) +
        first.k * (second.i * third.j - second.j * third.i);
}

Axes next(Axes a)
{
    if (a == Axes::x) return Axes::y;
    if (a == Axes::y) return Axes::z;
    if (a == Axes::z) return Axes::x;
    else return Axes::x;
}


real G(std::array<real, 2> locs, real& inv_stddev_2)
{
    return inv_stddev_2 / M_PI * exp(
        -inv_stddev_2 * ((locs[0] - 0.5) * (locs[0] - 0.5) + (locs[1] - 0.5) * (locs[1] - 0.5)));
}

Color Mean(std::vector<Color>& colors)
{
    int size = colors.size();
    Color mean = Color(0.0, 0.0, 0.0);
    for (int i = 0; i < size; i++) mean += colors[i];
    return mean / real(size);
}

real InvStdDev(Color& mean, std::vector<Color>& colors)
{
    // return 1.0 / (2.0 * 0.5 * 0.5);
    int size = colors.size();
    Color inv_stdev_2 = Color(0.0, 0.0, 0.0);
    for (int i = 0; i < size; i++) inv_stdev_2 += (colors[i] - mean) * (colors[i] - mean);
    // std::cout << inv_stdev_2 << std::endl;
    inv_stdev_2 = inv_stdev_2 / (size - 1);
    double res = 1.0 / (inv_stdev_2.r * inv_stdev_2.r + inv_stdev_2.g * inv_stdev_2.g + inv_stdev_2.b * inv_stdev_2.b);
    return std::max(std::min(res, 1.0), 0.1);
}


std::mt19937 gRandomGenerator(333);
std::uniform_real_distribution<> gNURandomDistribution(0, 0.99999f);

real getRandom()
{
    return gNURandomDistribution(gRandomGenerator);
}

std::mt19937 gRandomGeneratorTime;
std::uniform_real_distribution<> gNURandomDistributionTime(0, 1);

real getRandomTime()
{
    return gNURandomDistributionTime(gRandomGeneratorTime);
}

std::pair<int, int> closestFactors(int n)
{
    int a = static_cast<int>(std::sqrt(n));
    while (a > 0)
    {
        if (n % a == 0)
        {
            return {a, n / a};
        }
        a--;
    }
    return {1, n};
}


std::pair<Vec3r, Vec3r> getONB(Vec3r n)
{
    // orthogonal basis
    Vec3r u, v;
    real c = std::min(fabs(n.i), std::min(fabs(n.j), fabs(n.k)));
    if (c == fabs(n.i))
    {
        u.i = 0;
        u.j = -n.k;
        u.k = n.j;
    }
    else if (c == fabs(n.j))
    {
        u.j = 0;
        u.i = -n.k;
        u.k = n.i;
    }
    else if (c == fabs(n.k))
    {
        u.k = 0;
        u.j = -n.i;
        u.i = n.j;
    }
    u = u.normalize();
    v = x_product(n, u).normalize();
    return std::pair<Vec3r, Vec3r>(u, v);
}

real lerp(real t, real a, real b)
{
    return a + t * (b - a);
}

real lerp2D(Texel t, std::array<real, 4> a)
{
    real l1 = lerp(t.u, a[0], a[1]);
    real l2 = lerp(t.u, a[2], a[3]);
    return lerp(t.v, l1, l2);
}

real lerp3D(Vec3r t, std::array<real, 8> a)
{
    // assumes the vertices are in the order
    Texel t1 = {t.i, t.j};
    real l1 = lerp2D(t1, std::array<real,4>({a[0],a[1],a[2],a[3]}));
    real l2 = lerp2D(t1, std::array<real,4>({a[4],a[5],a[6],a[7]}));
    return lerp(t.k, l1, l2);
}

