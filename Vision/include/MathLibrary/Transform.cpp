//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: Transform.cpp
// Implementation of the class Transform.
// See Transform.hpp for more details.
//========================================================================  
 
// ----- Project Includes -----
#include <MathLibrary/Transform.hpp>

//============================================================= 
// xRotation
//=============================================================  
Transform xRotation(double theta)
{
 Transform xRotation;
 xRotation(1,1) = 1;
 xRotation(2,2) = cos(theta);
 xRotation(2,3) = -sin(theta);
 xRotation(3,2) = -xRotation(2,3);
 xRotation(3,3) = xRotation(2,2);
 return xRotation;
}


//============================================================= 
// yRotation
//=============================================================  
Transform yRotation(double theta)
{
 Transform yRotation;
 yRotation(1,1) = cos(theta);
 yRotation(1,3) = sin(theta);
 yRotation(2,2) = 1;
 yRotation(3,1) = -yRotation(1,3);
 yRotation(3,3) = yRotation(1,1);
 return yRotation;
}


//============================================================= 
// zRotation
//=============================================================  
Transform zRotation(double theta)
{
 Transform zRotation;
 zRotation(1,1) = cos(theta);
 zRotation(1,2) = -sin(theta);
 zRotation(2,1) = -zRotation(1,2);
 zRotation(2,2) = zRotation(1,1);
 zRotation(3,3) = 1;
 return zRotation;
}


//============================================================= 
// vectorRotation
//=============================================================  
Transform vectorRotation(ColumnVector<3, double> &vector, double theta)
{
 Transform vectorRotation;
 double ct = cos(theta);
 double st = sin(theta);
 Matrix<3, 3, double> rotation;
 rotation(1,1) = ct;
 rotation(1,2) = -vector(3)*st;
 rotation(1,3) = vector(2)*st;
 rotation(2,1) = -rotation(1,2);
 rotation(2,2) = rotation(1,1);
 rotation(2,3) = -vector(1)*st;
 rotation(3,1) = rotation(1,3);
 rotation(3,2) = -rotation(2,3);
 rotation(3,3) = rotation(1,1);
 rotation = (vector*transpose(vector)*(1-ct) + rotation);
 vectorRotation.setSubMatrix(1, 1, rotation);
 return vectorRotation;
}


