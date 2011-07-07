//---------------------------------------------------------------------//
//  Tensor3333.h: library for 3333-Tensor computation                  //
//                                                                     //
//  Programmed by Naoya Ohta (1998/11/11)                              //
//  Computer Science Department, Gunma University		       //
//  Modified for MathLibrary by Vilas Chitrakaran (2003/12/15)         //
//---------------------------------------------------------------------//

#ifndef _tensor3333_h_
#define _tensor3333_h_

#include <MathLibrary/VectorGSL.hpp>
#include "Math-Utility.hpp"
#include <stdio.h>
#include <iostream>

class Tensor3333
{
public:
  // constructors
  Tensor3333();
  Tensor3333(const Tensor3333&);

  // initializer
  void clear();

  // operators
  Tensor3333& operator=(const Tensor3333&);
  double& operator()(int, int, int, int);
  const double& operator()(int, int, int, int) const;

  // arithmetic operators
  Tensor3333& operator+=(const Tensor3333&);
  Tensor3333& operator-=(const Tensor3333&);
  Tensor3333& operator*=(double);
  Tensor3333& operator/=(double);

  friend Tensor3333 operator+(const Tensor3333&, const Tensor3333&);
  friend Tensor3333 operator-(const Tensor3333&, const Tensor3333&);
  friend Tensor3333 operator-(const Tensor3333&);
  friend Matrix<3,3> operator*(const Tensor3333&, const Matrix<3,3> &m);
  friend Tensor3333 operator*(double, const Tensor3333&);
  friend Tensor3333 operator*(const Tensor3333&, double);
  friend Tensor3333 operator/(const Tensor3333&, double);

private:
  double _t[81];
};

Matrix<9,9> type99(const Tensor3333 &); // 3333-Tensor -----> 99-martix
Tensor3333 type3333(const Matrix<9,9> &);   //             <-----
Matrix<6,6> type66(const Tensor3333 &); // (33)(33)-Tensor -> 66-martix
Tensor3333 type3333s(const Matrix<6,6> &);  //                 <-
VectorGSL<9> type9(const Matrix<3,3> &);      // 33-matrix -------> 9-vectorK
Matrix<3,3> type33(const VectorGSL<9> &);     //           <-------
VectorGSL<6> type6(const Matrix<3,3> &);      // (33)-matrix -----> 6-vectorK
Matrix<3,3> type33s(const VectorGSL<6> &);    //             <-----
VectorGSL<3> type3(const Matrix<3,3> &);      // [33]-matrix -----> 3-vectorK
Matrix<3,3> type33a(const VectorGSL<3> &);    //             <-----

// Tensor product of 33matrix and 33matrix
Tensor3333 tensprod(const Matrix<3,3> &, const Matrix<3,3> &);

// eigenvalues and eigenmatrices
void eigens(const Tensor3333 &t, Matrix<3,3> *ma, VectorGSL<9> &v);

// generalized inverse
Tensor3333 ginvs(const Tensor3333 &t, int rank);

#endif
