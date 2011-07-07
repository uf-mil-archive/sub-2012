
#ifndef INCLUDED_kin_math_hpp
#define INCLUDED_kin_math_hpp

#include <MathLibrary/Matrix.hpp>
#include <MathLibrary/VectorGSL.hpp>
#include <Homography.hpp>
#include <Tensor3333.hpp>
#include <Math-Utility.hpp>
#include <gsl/gsl_linalg.h>
#include <math.h>


//norm of a 2D vector
double norm(double n1, double n2);

//norm of a 3D vector
double norm(double n1, double n2, double n3);

//norm of a 3D GSL vector
double norm(VectorGSL<3> vin);

//turn rotation matrix into roll pitch yaw angles
void tr2rpy(Matrix<3,3> &R , VectorGSL<3> &rpy) ;

//convert rotation matrix to angle axis representation
void tr2utheta(Matrix<3,3> &R , double &theta, VectorGSL<3> &u) ;

#endif
