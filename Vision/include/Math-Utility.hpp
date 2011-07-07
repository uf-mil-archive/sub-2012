//======================================================================== 
// Package		: Homography and its decomposition
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2003
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: Math-Utility.hpp
// Interface to utility functions for the Math Library
//========================================================================  

#ifndef INCLUDED_Math_Utility_hpp
#define INCLUDED_Math_Utility_hpp

#include <MathLibrary/VectorGSL.hpp>
#include <math.h>
#include <gsl/gsl_eigen.h>

using namespace std;

//========================================================================  
// \brief
// Additional QMath utility functions.
//
// This file contains code originally developed by people such as
// Naoya Ohta and others in Dr Kenichi Kanatani's laboratory at 
// Okayama University and Gunma University. Their code has been modified
// for use with the Math Library
//========================================================================  

template<int numRows, int numColumns, class T>
T innerProduct(const Matrix<numRows, numColumns, T> &m1, const Matrix<numRows, numColumns, T> &m2);
	//  return  Inner product of matrices \m1 and \m2

template<class T>
Matrix<3,3, T> exteriorProduct(const Matrix<3,3, T> &m1, const Matrix<3,3, T> &m2);
	//  return  Exterior product of matrices \m1 and \m2

template<class T>
Matrix<3,3, T> exteriorProduct(const Matrix<3,3, T> &m, const VectorGSL<3, T> &v);
	//  return  Exterior product of matrix \m and vector \v

template<class T>
Matrix<3,3, T> exteriorProduct(const VectorGSL<3,T> &v, const Matrix<3,3, T> &m);
	//  return  Exterior product of matrix \m and vector \v

template<int size, class T>
Matrix<size, size, T> symmetrize(Matrix<size, size, T> &m);
	// Symmetrization of matrix (m + m^T).
	//  return A symmetric matrix

template<int size, class T>
Matrix<size, size, T> antiSymmetrize(Matrix<size, size, T> &m);
	// Anti-Symmetrization of matrix (m - m^T).
	//  return An anti-symmetric matrix

template<class T>
VectorGSL<3, T> skewSymmetricToVector(const Matrix<3, 3, T> &m);
	// Returns a 3x1 vector from its skew-symmetric matrix
	// representation \m.
	//  return A VectorGSL from its skew symmetric form 

template<int size, class T>
Matrix<size, size, T> ginvs(const Matrix<size, size, T> &m, int rk, double cn = 1.0e10);
	// Generalized inverse using spectral decomposition
	//  m  The matrix whose inverse we want to calculate
	//  rk Designated rank
	//  cn consition number
	//  return The inverse of the matrix

template<int size, class T>
int eigens(const Matrix<size, size, T> &matrix, Matrix<size, size, T> &evec, VectorGSL<size, T> &eval);
	// spectral decomposition for symmetric matrix
	//  matrix  input matrix
	//  evec  eigen vectors arranged as columns
	//  eval  eigen values of the matrix \matrix.
	//  return 0 on success, -1 on failure
	
int eeps(int, int, int);
	//  return  Eddington's epsilon.


//======================================================================== 
// innerProduct
//========================================================================  
template<int numRows, int numColumns, class T>
T innerProduct(const Matrix<numRows, numColumns, T> &m1, const Matrix<numRows, numColumns, T> &m2)
{
	T product;
	product = 0.0;
	for(int col = 1; col <= numColumns; col++)
		for(int row = 1; row <= numRows; row++)
			product += m1(col, row) * m2(col, row);

  return product;

}


//======================================================================== 
// exteriorProduct
//========================================================================  
template<class T>
Matrix<3,3, T> exteriorProduct(const Matrix<3,3, T> &m1, const Matrix<3,3, T> &m2)
{

  Matrix<3,3, T> n;

  n(1,1) = m1(2,2)*m2(3,3) - m1(2,3)*m2(3,2)
           - m1(3,2)*m2(2,3) + m1(3,3)*m2(2,2);
  n(1,2) = - m1(2,1)*m2(3,3) + m1(2,3)*m2(3,1)
           + m1(3,1)*m2(2,3) - m1(3,3)*m2(2,1);
  n(1,3) = m1(2,1)*m2(3,2) - m1(2,2)*m2(3,1)
           - m1(3,1)*m2(2,2) + m1(3,2)*m2(2,1);

  n(2,1) = - m1(1,2)*m2(3,3) + m1(1,3)*m2(3,2)
           + m1(3,2)*m2(1,3) - m1(3,3)*m2(1,2);
  n(2,2) = m1(1,1)*m2(3,3) - m1(1,3)*m2(3,1)
           - m1(3,1)*m2(1,3) + m1(3,3)*m2(1,1);
  n(2,3) = - m1(1,1)*m2(3,2) + m1(1,2)*m2(3,1)
           + m1(3,1)*m2(1,2) - m1(3,2)*m2(1,1);

  n(3,1) = m1(1,2)*m2(2,3) - m1(1,3)*m2(2,2)
           - m1(2,2)*m2(1,3) + m1(2,3)*m2(1,2);
  n(3,2) = - m1(1,1)*m2(2,3) + m1(1,3)*m2(2,1)
           + m1(2,1)*m2(1,3) - m1(2,3)*m2(1,1);
  n(3,3) = m1(1,1)*m2(2,2) - m1(1,2)*m2(2,1)
           - m1(2,1)*m2(1,2) + m1(2,2)*m2(1,1);
  return n;
}


template<class T>
Matrix<3,3, T> exteriorProduct(const Matrix<3,3, T> &m, const VectorGSL<3, T> &v)
{
	Matrix<3,3, T> n;

	for(int i = 0; i < 3; i++)
	{
		n(i+1, 1) = m(i+1, 3) * v(2) - m(i+1, 2) * v(3);
		n(i+1, 2) = m(i+1, 1) * v(3) - m(i+1, 3) * v(1);
		n(i+1, 3) = m(i+1, 2) * v(1) - m(i+1, 1) * v(2);
  	}
  	return n;
}

template<class T>
Matrix<3,3, T> exteriorProduct(const VectorGSL<3,T> &v, const Matrix<3,3, T> &m)
{
	Matrix<3,3,T> n;
	
	for(int i = 0; i < 3; i++)
	{
		n(1, i+1) = v(2) * m(3, i+1) - v(3) * m(2, i+1);
		n(2, i+1) = v(3) * m(1, i+1) - v(1) * m(3, i+1);
		n(3, i+1) = v(1) * m(2, i+1) - v(2) * m(1, i+1);
	}
	
	return n;
}


//======================================================================== 
// symmetrize
//========================================================================  
template<int size, class T>
Matrix<size, size, T> symmetrize(Matrix<size, size, T> &m)
{
  Matrix<size,size,T> out;
  out = (m + transpose(m))/2.0;
  return out;
}


//======================================================================== 
// antiSymmetrize
//========================================================================  
template<int size, class T>
Matrix<size, size, T> antiSymmetrize(Matrix<size, size, T> &m)
{
  Matrix<size,size,T> out;
  out = (m - transpose(m))/2.0;
  return out;
}


//======================================================================== 
// skewSymmetricToVectorGSL
//========================================================================  
template <class T>
VectorGSL<3, T> skewSymmetricToVector(const Matrix<3, 3, T> &m)
{
	VectorGSL<3, T> vector;
	vector = 0.0;
	
	if( (m(1,1) != 0.0) || (m(2,2) != 0.0) || (m(3,3) != 0.0) ||
     	(m(1,2) != -m(2,1)) || (m(1,3) != -m(3,1)) || (m(2,3) != -m(3,2)) )
	{
		static MathException exception;
		exception.isErrorTypeTypeMismatch();
		throw exception;
	}

	vector = m(3,2), m(1,3), m(2,1);
	return vector;
}


//======================================================================== 
// ginvs
//========================================================================  
template<int size, class T>
Matrix<size, size, T> ginvs(const Matrix<size, size, T> &m, int rk, double cn)
{
  int  i;

  // check designated rank
  if(rk < 1 || rk > size) {
    cerr << "ginvs: invalid rank." << endl;
    exit(1); }

  // check designated condition number
  if(cn < 1.0) {
    cerr << "ginvs: invalid condition number." << endl;
    exit(1); }

  // memory allocation for work buffers
  Matrix<size, size, T> u, diagV;
  VectorGSL<size, T> v;
  int idx[size];

  diagV = 0.0;
  
  // spectral decomposition
  eigens(m,u,v);

  // sort by absolute value of eigenvalues
  for(i = 0; i < size; i++) 
	idx[i] = i+1;
	
  for(i = 0; i < size-1; i++)
  	for(int j = i+1; j < size; j++) 
	{
	  double absi = v(idx[i]), absj = v(idx[j]);
      if(absi < 0.0) absi *= -1.0;
      if(absj < 0.0) absj *= -1.0;
      if(absj > absi) { int k = idx[i]; idx[i] = idx[j]; idx[j] = k; }
    }

  // check condition number
  double emax = v(idx[0]), emin = v(idx[rk-1]);
  if(emax < 0.0) emax *= -1.0;
  if(emin < 0.0) emin *= -1.0;
  if(emax >= cn*emin) {
    cerr << "ginvs: ill condition." << endl;
    exit(1); }
    
  // compute generalized inverse
  for(i = 0; i < rk; i++)
  	diagV(i+1,i+1) = 1.0/v(idx[i]);
  
  return u * diagV * transpose(u);
}


//======================================================================== 
// eigens
//========================================================================  
template<int size, class T>
int eigens(const Matrix<size, size, T> &matrix, Matrix<size, size, T> &evec, VectorGSL<size, T> &eval)
{
	gsl_eigen_symmv_workspace *gh_work;
	gsl_matrix *gh_mat;
	gsl_vector *gh_eval;
	gsl_matrix *gh_evec;

	gh_work = gsl_eigen_symmv_alloc(size);
	gh_mat = gsl_matrix_alloc(size,size);
	gh_eval = gsl_vector_alloc(size);
	gh_evec = gsl_matrix_alloc(size,size);
	
	for (int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			gsl_matrix_set(gh_mat, i, j, matrix(i+1,j+1));
		}
	}
	
	// Get the eigen vectors and sort them 
	if( gsl_eigen_symmv(gh_mat, gh_eval, gh_evec, gh_work) )
		return -1;
	if( gsl_eigen_symmv_sort(gh_eval, gh_evec, GSL_EIGEN_SORT_VAL_DESC) )
		return -1;
	
	for (int i = 0; i < size; i++)
	{
		eval(i+1) = gsl_vector_get(gh_eval, i);
		for(int j = 0; j < size; j++)
		{
			evec(i+1, j+1) = gsl_matrix_get(gh_evec, i, j);
		}
	}

	gsl_eigen_symmv_free(gh_work);
	gsl_matrix_free(gh_mat);
	gsl_matrix_free(gh_evec);
	gsl_vector_free(gh_eval);
	return 0;
}


#endif
