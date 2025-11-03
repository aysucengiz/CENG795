//
// Created by vicy on 11/03/25.
//

#include "Matrix.h"

M4trix::M4trix()
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            ptr[i][j]=0;
        }
    }
}

M4trix::M4trix(real a)
{
    for (int i=0; i<4; i++)
    {
        ptr[i][i]= a;
    }
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
            ptr[i][j]=other[i][j];
        }
    }
}

M4trix& M4trix::operator=(const M4trix& other)
{
    if (this == &other) return *this;
    for (int i=0; i<4; i++){for (int j=0; j<4; j++){ptr[i][j]=other[i][j];}}
    return *this;
}

M4trix& M4trix::operator+=(const M4trix& other)
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            ptr[i][j]+=other[i][j];
        }
    }
    return *this;
}

bool operator==(const M4trix& a, const M4trix& b)
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (fabs(a.ptr[i][j] - b.ptr[i][j]) > M4T_EPS)
                return false;
        }
    }
    return true;
}

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