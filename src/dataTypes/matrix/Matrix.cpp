//
// Created by vicy on 11/03/25.
//

#include "Matrix.h"
#include "../functions/overloads.h"



M4trix::M4trix()
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            arr[i][j]=0;
        }
    }
}

M4trix::M4trix(real a)
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (i== j) arr[i][j]= a;
            else arr[i][j]=0;
        }
    }
}

M4trix::M4trix(const std::array<std::array<double,4>,4>& p)
{
    arr = p;
}

M4trix Identity()
{
    return M4trix(1);
}


M4trix::M4trix(const M4trix& other)
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            arr[i][j]=other[i][j];
        }
    }
}

M4trix M4trix::Adjugate() const
{
    M4trix temp;
    temp[0][0] = (arr[1][1] * arr[2][2] * arr[3][3]) + (arr[1][2] * arr[2][3] * arr[3][1]) + (arr[1][3] * arr[2][1] * arr[3][2])
                -(arr[1][3] * arr[2][2] * arr[3][1]) - (arr[1][2] * arr[2][1] * arr[3][3]) - (arr[1][1] * arr[2][3] * arr[3][2]);

    temp[0][1] = (arr[0][1] * arr[2][2] * arr[3][3]) + (arr[0][2] * arr[2][3] * arr[3][1]) + (arr[0][3] * arr[2][1] * arr[3][2])
                -(arr[0][3] * arr[2][2] * arr[3][1]) - (arr[0][2] * arr[2][1] * arr[3][3]) - (arr[0][1] * arr[2][3] * arr[3][2]);
    temp[0][1]  = -temp[0][1];

    temp[0][2] = (arr[0][1] * arr[1][2] * arr[3][3]) + (arr[0][2] * arr[1][3] * arr[3][1]) + (arr[0][3] * arr[1][1] * arr[3][2])
                -(arr[0][3] * arr[1][2] * arr[3][1]) - (arr[0][2] * arr[1][1] * arr[3][3]) - (arr[0][1] * arr[1][3] * arr[3][2]);

    temp[0][3] = (arr[0][1] * arr[1][2] * arr[2][3]) + (arr[0][2] * arr[1][3] * arr[2][1]) + (arr[0][3] * arr[1][1] * arr[2][2])
                -(arr[0][3] * arr[1][2] * arr[2][1]) - (arr[0][2] * arr[1][1] * arr[2][3]) - (arr[0][1] * arr[1][3] * arr[2][2]);
    temp[0][3]  = -temp[0][3];

    temp[1][0] = (arr[1][0] * arr[2][2] * arr[3][3]) + (arr[1][2] * arr[2][3] * arr[3][0]) + (arr[1][3] * arr[2][0] * arr[3][2])
                -(arr[1][3] * arr[2][2] * arr[3][0]) - (arr[1][2] * arr[2][0] * arr[3][3]) - (arr[1][0] * arr[2][3] * arr[3][2]);
    temp[1][0] = -temp[1][0];

    temp[1][1] = (arr[0][0] * arr[2][2] * arr[3][3]) + (arr[0][2] * arr[2][3] * arr[3][0]) + (arr[0][3] * arr[2][0] * arr[3][2])
                -(arr[0][3] * arr[2][2] * arr[3][0]) - (arr[0][2] * arr[2][0] * arr[3][3]) - (arr[0][0] * arr[2][3] * arr[3][2]);

    temp[1][2] = (arr[0][0] * arr[1][2] * arr[3][3]) + (arr[0][2] * arr[1][3] * arr[3][0]) + (arr[0][3] * arr[1][0] * arr[3][2])
                -(arr[0][3] * arr[1][2] * arr[3][0]) - (arr[0][2] * arr[1][0] * arr[3][3]) - (arr[0][0] * arr[1][3] * arr[3][2]);
    temp[1][2] = -temp[1][2];

    temp[1][3] = (arr[0][0] * arr[1][2] * arr[2][3]) + (arr[0][2] * arr[1][3] * arr[2][0]) + (arr[0][3] * arr[1][0] * arr[2][2])
                -(arr[0][3] * arr[1][2] * arr[2][0]) - (arr[0][2] * arr[1][0] * arr[2][3]) - (arr[0][0] * arr[1][3] * arr[2][2]);


    temp[2][0] = (arr[1][0] * arr[2][1] * arr[3][3]) + (arr[1][1] * arr[2][3] * arr[3][0]) + (arr[1][3] * arr[2][0] * arr[3][1])
                -(arr[1][3] * arr[2][1] * arr[3][0]) - (arr[1][1] * arr[2][0] * arr[3][3]) - (arr[1][0] * arr[2][3] * arr[3][1]);

    temp[2][1] = (arr[0][0] * arr[2][1] * arr[3][3]) + (arr[0][1] * arr[2][3] * arr[3][0]) + (arr[0][3] * arr[2][0] * arr[3][1])
                -(arr[0][3] * arr[2][1] * arr[3][0]) - (arr[0][1] * arr[2][0] * arr[3][3]) - (arr[0][0] * arr[2][3] * arr[3][1]);
    temp[2][1] = -temp[2][1];

    temp[2][2] = (arr[0][0] * arr[1][1] * arr[3][3]) + (arr[0][1] * arr[1][3] * arr[3][0]) + (arr[0][3] * arr[1][0] * arr[3][1])
                -(arr[0][3] * arr[1][1] * arr[3][0]) - (arr[0][1] * arr[1][0] * arr[3][3]) - (arr[0][0] * arr[1][3] * arr[3][1]);

    temp[2][3] = (arr[0][0] * arr[1][1] * arr[2][3]) + (arr[0][1] * arr[1][3] * arr[2][0]) + (arr[0][3] * arr[1][0] * arr[2][1])
                -(arr[0][3] * arr[1][1] * arr[2][0]) - (arr[0][1] * arr[1][0] * arr[2][3]) - (arr[0][0] * arr[1][3] * arr[2][1]);
    temp[2][3] = -temp[2][3];


    temp[3][0] = (arr[1][0] * arr[2][1] * arr[3][2]) + (arr[1][1] * arr[2][2] * arr[3][0]) + (arr[1][2] * arr[2][0] * arr[3][1])
                -(arr[1][2] * arr[2][1] * arr[3][0]) - (arr[1][1] * arr[2][0] * arr[3][2]) - (arr[1][0] * arr[2][2] * arr[3][1]);
    temp[3][0] = -temp[3][0];

    temp[3][1] = (arr[0][0] * arr[2][1] * arr[3][2]) + (arr[0][1] * arr[2][2] * arr[3][0]) + (arr[0][2] * arr[2][0] * arr[3][1])
                -(arr[0][2] * arr[2][1] * arr[3][0]) - (arr[0][1] * arr[2][0] * arr[3][2]) - (arr[0][0] * arr[2][2] * arr[3][1]);

    temp[3][2] = (arr[0][0] * arr[1][1] * arr[3][2]) + (arr[0][1] * arr[1][2] * arr[3][0]) + (arr[0][2] * arr[1][0] * arr[3][1])
                -(arr[0][2] * arr[1][1] * arr[3][0]) - (arr[0][1] * arr[1][0] * arr[3][2]) - (arr[0][0] * arr[1][2] * arr[3][1]);
    temp[3][2] = -temp[3][2];

    temp[3][3] = (arr[0][0] * arr[1][1] * arr[2][2]) + (arr[0][1] * arr[1][2] * arr[2][0]) + (arr[0][2] * arr[1][0] * arr[2][1])
                -(arr[0][2] * arr[1][1] * arr[2][0]) - (arr[0][1] * arr[1][0] * arr[2][2]) - (arr[0][0] * arr[1][2] * arr[2][1]);

    return temp;
}


real M4trix::Determinant() const
{
    Vec3r v1(arr[1][0], arr[2][0], arr[3][0]);
    Vec3r v2(arr[1][1], arr[2][1], arr[3][1]);
    Vec3r v3(arr[1][2], arr[2][2], arr[3][2]);
    Vec3r v4(arr[1][3], arr[2][3], arr[3][3]);
    /*std::cout << "v1: " << v1 << std::endl;
    std::cout << "v2: " << v2 << std::endl;
    std::cout << "v3: " << v3 << std::endl;
    std::cout << "v4: " << v4 << std::endl;*/
    return arr[0][0] * determinant(v2,v3,v4)
         - arr[0][1] * determinant(v1,v3,v4)
         + arr[0][2] * determinant(v1,v2,v4)
         - arr[0][3] * determinant(v1,v2,v3);
}


M4trix M4trix::Inverse() const
{
    double x = Determinant();
    return Adjugate() * (1.0/Determinant());
}

M4trix M4trix::Transpose()
{
    M4trix temp;
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            temp[i][j] = arr[j][i];
        }
    }
    return temp;
}



M4trix& M4trix::operator=(const M4trix& other)
{
    if (this == &other) return *this;
    for (int i=0; i<4; i++){for (int j=0; j<4; j++){arr[i][j]=other[i][j];}}
    return *this;
}

M4trix& M4trix::operator+=(const M4trix& other)
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            arr[i][j]+=other[i][j];
        }
    }
    return *this;
}

std::array<double,4>& M4trix::operator[](int i) { return arr[i]; }
const std::array<double,4>& M4trix::operator[](int i) const { return arr[i]; }

// M4trix

M4trix operator+(const M4trix& a, const M4trix& b)
{
    M4trix c;
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            c[i][j] = a[i][j]+b[i][j];
        }
    }
    return c;
}
M4trix operator-(const M4trix& a, const M4trix& b)
{
    M4trix c;
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            c[i][j] = a[i][j]-b[i][j];
        }
    }
    return c;
}

M4trix operator*(const M4trix& a, const M4trix& b)
{
    M4trix c;
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            c[i][j] = 0.0;
            for (int k=0; k<4; k++)
            {
                c[i][j] += a[i][k]*b[k][j];
            }
        }
    }
    return c;
}

std::ostream& operator<<(std::ostream& os, const M4trix& a)
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            os << a[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}



bool operator==(const M4trix& a, const M4trix& b)
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (fabs(a.arr[i][j] - b.arr[i][j]) > M4T_EPS)
                return false;
        }
    }
    return true;
}


Vec4r operator *(const M4trix &a, const Vec4r &b)
{
    Vec4r c(0,0,0,0);

        c.x += a[0][0]*b.x + a[0][1]*b.y + a[0][2]*b.z + a[0][3]*b.w;
        c.y += a[1][0]*b.x + a[1][1]*b.y + a[1][2]*b.z + a[1][3]*b.w;
        c.z += a[2][0]*b.x + a[2][1]*b.y + a[2][2]*b.z + a[2][3]*b.w;
        c.w += a[3][0]*b.x + a[3][1]*b.y + a[3][2]*b.z + a[3][3]*b.w;

    return c;
}

Ray operator *(const M4trix &a, const Ray &b)
{
    Ray r;
    r.dir = (a * Vec4r(b.dir)).getVec3r();
    r.pos = (a * Vec4r(b.pos)).getVertex();
    return r;
}

