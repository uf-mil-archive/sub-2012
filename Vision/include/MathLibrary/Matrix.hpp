//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: Matrix.hpp
// Interface of the class Matrix.
//========================================================================  
 
#ifndef INCLUDED_Matrix_hpp
#define INCLUDED_Matrix_hpp

#include "MatrixBase.hpp" 
#include "VectorBase.hpp"

#include <iostream>
#include <iomanip>
#include <math.h>
 
using namespace std;

//====================================================================
// class Matrix
// -------------------------------------------------------------------
// \brief
// Methods for mathematical operations on matrices.
//
// The class \c Matrix is derived from its base class \c MatrixBase.
//
// This class provides common mathematical functions for matrices such 
// as addition, multipication, division and subtraction between matrices, 
// along with methods to get/set elements/sub-matrices. The template class 
// also provides methods for determination of the inverse of a matrix upto 
// 4 x 4, the transpose of a matrix and generation of unit matrices. 
// The classes \c ColumnVector, \c RowVector and \c Transform are 
// derived from this class.
//
// <b>Example Program:</b>
//
// \include Matrix.t.cpp
//========================================================================  

template<int r, int c, class T> class Matrix;
template<int n, class T> class ColumnVector; 
template<int n, class T> class RowVector;
template<class T>class MatrixInitializer;

// Pre-declaration of friend functions follow.

template<int r, int c, class T> ostream &operator<< (ostream &out, const Matrix<r,c,T> &m);
template<int r, int c, class T> istream &operator>> (istream &in, Matrix<r,c,T> &m);
template<int r, int c, class T> bool operator==(const Matrix<r,c,T> &lhs, const Matrix<r,c,T> &rhs);
template<int r, int c, class T> bool operator!=(const Matrix<r,c,T> &lhs, const Matrix<r,c,T> &rhs);


template<int numRows, int numColumns, class T = double>
class Matrix : public MatrixBase<T>
{
 public:
  inline Matrix() {}
   // The default constructor for the \c Matrix object.
   // The elements of Matrix are not initialized.
	
  inline Matrix(const Matrix<numRows, numColumns, T> &matrix);
   // Copy Constructor.

  virtual ~Matrix() {}
   // The default destructor of the \c Matrix object.
	
  virtual T *getElementsPointer() const { return (T *) &d_element[0][0]; }
   //  return  The pointer to the first element in the Matrix.
		
  virtual int getNumRows() const {return numRows;}
   //  return  The number of rows in the Matrix.

  virtual int getNumColumns() const {return numColumns;}
   //  return 	The number of columns in the Matrix.
	
  inline ColumnVector<numRows, T> getColumn(int c) const;
   //  return  The column specified by \a c.

  //inline ColumnVector<numRows, T> setColumn(int c) const;
   //  sets  The column specified by \a c.
 
  inline RowVector<numColumns, T> getRow(int r) const;
   //  return  The row specified by \a r.

  //inline RowVector<numColumns, T> setRow(int r) const;
   //  sets  The row specified by \a r.

  inline virtual T getElement(int r, int c) const;
   // returns the element at the specified position.
   //  r  Row number of the desired element.
   //  c  Column number of the desired element.

  inline virtual void setElement(int r, int c, T val);
   // Sets an element to a value at the specified position.
   //  r    Row number of the desired element.
   //  c    Column number of the desired element.
   //  val  The desired element is set to this value.

  template<int sr, int sc, class X>
  void getSubMatrix(int pivotRow, int pivotColumn, Matrix<sr, sc, X> &m) const; 
   // This function extracts a sub-matrix of the size of \a m (sr x sc) starting 
   // from the pivotal row and column.
   //  pivotRow, pivotColumn  The position of the first element of the sub-matrix 
   //                         in the matrix from which it is extracted.
   //  m                      The extracted sub matrix.
																
  template<int sr, int sc, class X>
  void setSubMatrix(int pivotRow, int pivotColumn, const Matrix<sr, sc, X> &m);
   // This function sets a sub-matrix of the size of \a m (sr x sc) starting from 
   // the pivotal row and column of a matrix.
   //  pivotRow, pivotColumn  The position of the first element	of the sub-matrix 
   //                         in the matrix to  which it is extracted.
   //  m                      The sub-matrix.				
	
  inline T &operator()(int r, int c); 
   // Access or assign the element at \a r row and \a c column of the matrix.
   // Example: \code myMatrix(2,3)=22.2; \endcode
			
  inline T operator()(int row, int column) const;
   // Access the element at \a r row and \a c column of the matrix.
   // Example: \code cout << myMatrix(2,3); \endcode.

  inline MatrixInitializer<T> operator=(const T &val);
   // Assignment operator for initializing a Matrix object.
   //  val  This is the value to which all elements in the matrix
   //       are initialized.
   // The initialization of the Matrix object can also
   // be done as a comma seperated list. For example:
   // \code Matrix<2,2> myMatrix;
   // myMatrix = 67.899, 23.45, 6, 98; \endcode

  inline Matrix &operator=(const MatrixBase<T> &m);
   // Assign a \a MatrixBase type to a \a Matrix type. The dimensions 
   // of both the objects must be the same.
   //  m  The object of the base class \a MatrixBase.

  inline Matrix operator+(const Matrix<numRows, numColumns, T> &rhs);
   // Matrix addition operator.
   //  rhs     The right hand side Matrix.
   //  return  The sum of \a rhs and the matrix to the left hand side 
   //          of the addition operator.

  inline Matrix operator-(const Matrix<numRows, numColumns, T> &rhs);
   // Matrix difference operator.
   //  rhs     The right hand side Matrix.
   //  return  The matrix after subtracting \a rhs matrix from the matrix 
   //          on the left hand side of the difference operator.

  inline Matrix operator*(const T &scalar);
   // Post-multiplication of a matrix with a scalar.
   //  scalar  The scalar value to be multiplied with the matrix.
   //  return  The product of \a scalar and the matrix.

  inline Matrix operator/(const T &scalar);
   // Division of a matrix by a scalar.
   //  scalar  The scalar value to divide the Matrix with.
   //  return  The matrix with each element divided
   //          by the \a scalar.
	
  friend ostream &operator<< <>(ostream &output, const Matrix<numRows, numColumns, T> &matrix);
   // This function overloads the ostream \c << operator
   // to output the elements of the matrix \a matrix row-wise to the output stream
   // separated by white spaces(e.g. spaces).
   // Example: \code cout << matrix; \endcode

  friend istream &operator>> <>(istream &input, Matrix<numRows, numColumns, T> &matrix);
   // This function overloads the istream \c >> operator
   // to read the elements of the matrix \a matrix from an input stream.
   // The elements must be arranged row-wise in the input stream, separated
   // by white spaces (e.g. spaces, tabs, etc).
   // Example: \code cin >> matrix; \endcode	

  friend bool operator== <>(const Matrix<numRows, numColumns, T> &lhs, const Matrix<numRows, numColumns, T> &rhs);
   //  return  TRUE if the \a lhs matrix is same as the \a rhs matrix, else FALSE.	

  friend bool operator!= <>(const Matrix<numRows, numColumns, T> &lhs, const Matrix<numRows, numColumns, T> &rhs);
   //  return  TRUE if the \a lhs matrix is not the same as the \a rhs matrix, else FALSE.	

 protected:
  T d_element[numRows][numColumns];
};

//================================================
// Declaration of template functions.
//================================================

template<int r1, int c1r2, int c2, class T>
Matrix<r1, c2, T> operator* (const Matrix<r1, c1r2, T> &m1, const Matrix<c1r2, c2, T> &m2);
 // Matrix multiplication.
 //  return  The product of \a m1 and \a m2.

template<int c1r2, class T>
T operator* (const Matrix<1, c1r2, T> &m1, const Matrix<c1r2, 1, T> &m2);
 // Multiplication between a row Matrix object and a column Matrix.
 //  return  The scalar product of row matrix \a m1 and column matrix \a m2. 
 
template<int numRows, int numColumns, class T>
Matrix<numRows, numColumns, T> operator*(const int &scalar, const Matrix<numRows, numColumns, T> &m);
 // Pre-multiplication of a matrix with scalar of type \c integer.
 //  return  The product of \a scalar and \a matrix.

template<int numRows, int numColumns, class T>
Matrix<numRows, numColumns, T> operator*(const double &scalar, const Matrix<numRows, numColumns, T> &m);
 // Pre-multiplication of a matrix with scalar of type \c double.
 //  return  The product of \a scalar and \a matrix.

template<int numColumns, int numRows, class T>
Matrix<numColumns, numRows, T> transpose(const Matrix<numRows, numColumns, T> &matrix);
 //  return The transpose of the matrix \a matrix.

template<int size, class T>
Matrix<size, size, T> inverse(const Matrix<size, size, T> &m);
 // Inverse of a square matrix \a m. This function can be used to 
 // compute inverses of matrices of size 6x6, 8x8, 12x12, etc 
 // (dimensions that are even multiples of 2, 3, 4).

template<int size, class T> 
T determinant ( Matrix <size, size, T> &matrix);
 //  return  Determinant of a matrix.
 
template<int size, class T> 
T trace( Matrix <size, size, T> &matrix);
 //  return Trace of a size x size matrix.

template<int size, class T>
Matrix<size, size, T> unitMatrix();
 // Generate a unit matrix of size x size.
 // Example: \code Matrix<3,3> A;
 // A=unitMatrix<3>(); \endcode

// ========== END OF INTERFACE ==========


//===========================================================================
// Definition of inlined functions.
//===========================================================================

//======================================================================== 
// Matrix::Matrix: Constructor of the Matrix class
//========================================================================  
template<int nr, int nc, class T>
Matrix<nr, nc, T>::Matrix(const Matrix<nr, nc, T> &m)
{
 for (int r = 0; r < nr; r++)
  for (int c = 0; c < nc; c++)
   d_element[r][c] = m.d_element[r][c];
}	


//======================================================================== 
// Matrix::getElement
//========================================================================  
template<int nr, int nc, class T>
T Matrix<nr, nc, T>::getElement(int r, int c) const
{
 if( (r > nr) || (r < 1) || (c > nc) || (c < 1) )
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 return d_element[r-1][c-1];
}		


//======================================================================== 
// Matrix::setElement
//========================================================================  
template<int nr, int nc, class T>
void Matrix<nr, nc, T>::setElement(int r, int c, T val) 
{
 if( (r > nr) || (r < 1) || (c > nc) || (c < 1) )
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 d_element[r-1][c-1] = val;
}

//======================================================================== 
// Matrix::getColumn
//========================================================================  

template<int nr, int nc, class T>
ColumnVector<nr, T> Matrix<nr, nc, T>::getColumn(int c) const
{
 if( (c > nc) || (c < 1) )
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 ColumnVector<nr, T> column;
 for (int p = 1; p <= nr; p++)
  column(p) = d_element[p-1][c-1];
 return column;
}

//======================================================================== 
// Matrix::setColumn
//========================================================================  

//template<int nr, int nc, class T>
//ColumnVector<nr, T> Matrix<nr, nc, T>::setColumn(int c) const
//{
// if( (c > nc) || (c < 1) )
// {
//  static MathException exception;
//  exception.setErrorTypeIndexOutOfRange();
//  throw exception;
// }
// ColumnVector<nr, T> column;
// for (int p = 1; p <= nr; p++)
//  d_element[p-1][c-1] = column(p);
// return column;
//}
 	
//======================================================================== 
// Matrix::getRow
//========================================================================  
template<int nr, int nc, class T>
RowVector<nc, T> Matrix<nr, nc, T>::getRow(int r) const
{	
 if( (r < 1) || (r > nr) )
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 
 RowVector<nc, T> row;
 for (int p = 1; p <= nc; p++) 
  row(p) = d_element[r-1][p-1];
 return row;
}


//======================================================================== 
// Matrix::setRow
//========================================================================  
//template<int nr, int nc, class T>
//RowVector<nc, T> Matrix<nr, nc, T>::setRow(int r) const
//{	
// if( (r < 1) || (r > nr) )
// {
//  static MathException exception;
//  exception.setErrorTypeIndexOutOfRange();
//  throw exception;
// }
// 
// RowVector<nc, T> row;
// for (int p = 1; p <= nc; p++) 
//  d_element[r-1][p-1] = row(p);
// return row;
//}

//======================================================================== 
// Matrix::operator()
//========================================================================  
template<int nr, int nc, class T>
T &Matrix<nr, nc, T>::operator()(int r, int c)
{
 if( (r > nr) || (r < 1) || (c > nc) || (c < 1) )
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 return d_element[r-1][c-1];
}


template<int nr, int nc, class T>
T Matrix<nr, nc, T>::operator()(int r, int c) const 
{
 if( (r > nr) || (r < 1) || (c > nc) || (c < 1) )
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 return d_element[r-1][c-1];
}


//======================================================================== 
// Matrix::operator=
//========================================================================  
template<int nr, int nc, class T>
Matrix<nr, nc, T> &Matrix<nr, nc, T>::operator=(const MatrixBase<T> &m)
{
 if ( (nr != m.getNumRows()) || (nc != m.getNumColumns()) )
 {
  static MathException exception;
  exception.setErrorTypeIncompatibleSize();
  throw exception;
 }
 
 for (int r = 0; r < nr; r++)
  for (int c = 0; c < nc; c++)
   d_element[r][c] = m.getElement(r+1, c+1);
 return *this;
}


template<int nr, int nc, class T>
MatrixInitializer<T> Matrix<nr, nc, T>::operator=(const T &val)
{
 for (int r = 0; r < nr; r++)
  for (int c = 0; c < nc; c++)
   d_element[r][c] = val;
 MatrixInitializer<T>  matrixInitialize(nr*nc, 1, *d_element);
 return matrixInitialize;
}


//======================================================================== 
// Matrix::operator+
//========================================================================  
template<int nr, int nc, class T>
Matrix<nr, nc, T> Matrix<nr, nc, T>::operator+(const Matrix<nr, nc, T> &rhs)
{
 Matrix<nr, nc, T> sum;
 for (int r = 0; r < nr; r++)
  for (int c = 0; c < nc; c++)
   sum.d_element[r][c] = d_element[r][c] + rhs.d_element[r][c];
 return (sum);
}


//======================================================================== 
// Matrix::operator-
//========================================================================  

template<int nr, int nc, class T>
Matrix<nr, nc, T> Matrix<nr, nc, T>::operator-(const Matrix<nr, nc, T> &rhs)
{
 Matrix<nr, nc, T> diff;
 for (int r = 0; r < nr; r++)
  for (int c = 0; c < nc; c++)
   diff.d_element[r][c] = d_element[r][c] - rhs.d_element[r][c];
 return (diff);
}


//======================================================================== 
// Matrix::operator*
//========================================================================  
template<int nr, int nc, class T>
Matrix<nr, nc, T> Matrix<nr, nc, T>::operator*(const T &s)
{
 Matrix<nr,nc, T> p;
 for (int r = 0; r < nr; r++)
  for (int c = 0; c < nc; c++)
   p.d_element[r][c] = d_element[r][c] * s; 			
 return p;
}


template<int nr, int nc, class T>
Matrix<nr, nc, T> operator*(const int &s, const Matrix<nr, nc, T> &m)
{
 Matrix<nr,nc, T> p;
 for (int r = 1; r <= nr; r++)
  for (int c = 1; c <= nc; c++)
   p(r,c) = m(r,c) * s; 			
 return p;
}


template<int nr, int nc, class T>
Matrix<nr, nc, T> operator*(const double &s, const Matrix<nr, nc, T> &m)
{
 Matrix<nr,nc, T> p;
 for (int r = 1; r <= nr; r++)
  for (int c = 1; c <= nc; c++)
   p(r,c) = m(r,c) * s; 			
 return p;
}


template<int r1, int c1r2, int c2, class T>
Matrix<r1, c2, T> operator* (const Matrix<r1, c1r2, T> &m1, const Matrix<c1r2, c2, T> &m2)
{
 Matrix<r1, c2, T> p;
 T pe;
 for (int r = 1; r <= r1; r++)
 {
  for (int c = 1; c <= c2; c++)
  {
   pe = 0;
   for (int com = 1; com <= c1r2; com++)
    pe += m1(r, com) * m2(com, c);
   p(r, c) = pe;
  }
 }
 return p;
}


template<int com, class T>
T operator* (const Matrix<1, com, T> &m1, const Matrix<com,1, T> &m2)
{
 T pe = 0;
 for (int c = 1; c <= com; c++)
  pe += m1(1, c) * m2(c, 1);
 return pe;
}


//======================================================================== 
// Matrix::operator/
//========================================================================  
template<int nr, int nc, class T>
Matrix<nr, nc, T> Matrix<nr, nc, T>:: operator/(const T &s)
{
 if (s == 0)
 {
  static MathException exception;
  exception.setErrorTypeDivideByZero();
  throw exception;
 }
 
 Matrix<nr,nc, T> m;
 for (int r = 0; r < nr; r++)
  for (int c = 0; c < nc; c++)
   m.d_element[r][c] = d_element[r][c]/s; 			
 return m;
}


//======================================================================== 
// transpose
//========================================================================  
template<int nc, int nr, class T>
Matrix<nc, nr, T> transpose(const Matrix<nr, nc, T> &m)
{
 Matrix<nc, nr, T> t;
 for (int r = 1; r <= nr; r++)
  for (int c = 1; c <= nc; c++)
   t(c, r) = m(r, c);
 return t;
}


//======================================================================== 
// unitMatrix
//========================================================================  
template<int size>
Matrix<size,size,double> unitMatrix()
{
 Matrix<size, size> u;
 for (int r = 1; r <= size; r++)
  for (int c = 1; c <= size; c++)
   (r == c) ? (u(r, c) = 1) : (u(r, c) = 0);
 return u;
}


//======================================================================== 
// Matrix::getSubMatrix
//========================================================================  
template<int nr, int nc, class T>
template<int snr, int snc, class X>
void Matrix<nr, nc, T>::getSubMatrix(int pr, int pc, Matrix<snr, snc, X> &sm) const
{
 if( (pr < 1) || (pr > nr) || (pc < 1) || (pc > nc))
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 if( (snc > (nc - pc + 1)) || (snr > (nr - pr + 1)) )
 {
  static MathException exception;
  exception.setErrorTypeIncompatibleSize();
  throw exception;
 }
 
 int c;
 int r = pr;
 for (int sr = 1; sr <= snr; sr++)
 {
  c = pc;
  for (int sc = 1; sc <= snc; sc++)
  {
   sm(sr, sc) = d_element[r-1][c-1];
   c++;
  }
  r++;
 }
}


//======================================================================== 
// Matrix::setSubMatrix
//========================================================================  
template<int nr, int nc, class T>
template<int snr, int snc, class X>
void Matrix<nr, nc, T>::setSubMatrix(int pr, int pc, const Matrix<snr, snc, X> &sm)
{
 if( (pr < 1) || (pr > nr) || (pc < 1) || (pc > nc))
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 if( (snc > (nc - pc + 1)) || (snr > (nr - pr + 1)) )
 {
  static MathException exception;
  exception.setErrorTypeIncompatibleSize();
  throw exception;
 }
 
 int c;
 int r = pr;
 for (int sr = 1; sr <= snr; sr++)
 {
  c = pc;
  for (int sc = 1; sc <= snc; sc++)
  {
   d_element[r-1][c-1] = sm(sr, sc);
   c++;
  }	
  r++;
 }
}


//======================================================================== 
// operator<<
//========================================================================  
template<int nr, int nc, class T>
ostream &operator<< (ostream &out, const Matrix<nr, nc, T> &m)
{
 for (int r = 0; r < nr; r++)
 { 
  for (int c = 0; c < nc; c++)
  {
   out << m.d_element[r][c] << " ";
  }
  if (r < (nr-1)) out << "\n";
 }
 return out;
}


//======================================================================== 
// operator>>
//========================================================================  
template<int nr, int nc, class T>
istream &operator>> (istream &in, Matrix<nr, nc, T> &m)
{
 for(int r = 0; r < nr; r++)
 {
  for(int c = 0; c < nc; c++)
  {
   while((cin.peek()=='\n')||(cin.peek()=='\r')||(cin.peek()==' '))
    cin.ignore(1);
   in >> m.d_element[r][c];
  }
 }
 return (in);
}					


//======================================================================== 
// operator==
//========================================================================  
template<int nr, int nc, class T>
bool operator==(const Matrix<nr, nc, T> &lhs, const Matrix<nr, nc, T> &rhs)
{
 for(int r = 0; r < nr; r++)
 {
  for(int c = 0; c < nc; c++)
  {
   if(lhs.d_element[r][c] != rhs.d_element[r][c])
    return false;
  }
 }
 return true;
}


//======================================================================== 
// operator!=
//========================================================================  
template<int nr, int nc, class T>
bool operator!=(const Matrix<nr, nc, T> &lhs, const Matrix<nr, nc, T> &rhs)
{
 if(lhs == rhs)
  return false;
 return true;
}

//======================================================================== 
// inverse
//========================================================================  

template<class T>
Matrix<2, 2, T> inverse(const Matrix<2, 2, T> &m)
{
 Matrix<2,2,T> A; // This is the adjoint of matrix m.
 T det = (m(1,1)*m(2,2)-m(1,2)*m(2,1));
	
 if (fabs(det) < 1e-10)
 {
  static MathException exception;
  exception.setErrorTypeSingularMatrix();
  throw exception;
 }
 A(1,1) = m(2,2);
 A(1,2) = -m(1,2);
 A(2,1) = -m(2,1);
 A(2,2) = m(1,1);
 return (A/det);
}


template<class T>
Matrix<3, 3, T> inverse(const Matrix<3, 3, T> &m)
{
 Matrix<3,3,T> A;	// This is the adjoint of matrix m;
 T det = (m(1,1)*m(2,2)*m(3,3)-m(1,1)*m(2,3)*m(3,2)-m(2,1)*m(1,2)*m(3,3)+m(2,1)
         *m(1,3)*m(3,2)+m(3,1)*m(1,2)*m(2,3)-m(3,1)*m(1,3)*m(2,2));

 if (fabs(det) < 1e-10)
 {
  static MathException exception;
  exception.setErrorTypeSingularMatrix();
  throw exception;
 }
 A(1,1) = (m(2,2)*m(3,3)-m(2,3)*m(3,2));
 A(1,2) = -(m(1,2)*m(3,3)-m(1,3)*m(3,2));
 A(1,3) = (m(1,2)*m(2,3)-m(1,3)*m(2,2));
 A(2,1) = -(m(2,1)*m(3,3)-m(2,3)*m(3,1));
 A(2,2) = (m(1,1)*m(3,3)-m(1,3)*m(3,1));
 A(2,3) = -(m(1,1)*m(2,3)-m(1,3)*m(2,1));
 A(3,1) = (m(2,1)*m(3,2)-m(2,2)*m(3,1));
 A(3,2) = -(m(1,1)*m(3,2)-m(1,2)*m(3,1));
 A(3,3) = (m(1,1)*m(2,2)-m(1,2)*m(2,1));
 return (A/det);
}
	

template<class T>
Matrix<4, 4, T> inverse(const Matrix<4, 4, T> &m)
{
 Matrix<4,4,T> A;	// This is the adjoint matrix;
 T det = (m(1,1)*m(2,2)*m(3,3)*m(4,4)-m(1,1)*m(2,2)*m(3,4)*m(4,3)-m(1,1)*m(3,2)*m(2,3)*m(4,4)+m(1,1)*m(3,2)*m(2,4)*m(4,3)+m(1,1)
         *m(4,2)*m(2,3)*m(3,4)-m(1,1)*m(4,2)*m(2,4)*m(3,3)-m(2,1)*m(1,2)*m(3,3)*m(4,4)+m(2,1)*m(1,2)*m(3,4)*m(4,3)+m(2,1)*m(3,2)
	 *m(1,3)*m(4,4)-m(2,1)*m(3,2)*m(1,4)*m(4,3)-m(2,1)*m(4,2)*m(1,3)*m(3,4)+m(2,1)*m(4,2)*m(1,4)*m(3,3)+m(3,1)*m(1,2)*m(2,3)
	 *m(4,4)-m(3,1)*m(1,2)*m(2,4)*m(4,3)-m(3,1)*m(2,2)*m(1,3)*m(4,4)+m(3,1)*m(2,2)*m(1,4)*m(4,3)+m(3,1)*m(4,2)*m(1,3)*m(2,4)
	 -m(3,1)*m(4,2)*m(1,4)*m(2,3)-m(4,1)*m(1,2)*m(2,3)*m(3,4)+m(4,1)*m(1,2)*m(2,4)*m(3,3)+m(4,1)*m(2,2)*m(1,3)*m(3,4)-m(4,1)
	 *m(2,2)*m(1,4)*m(3,3)-m(4,1)*m(3,2)*m(1,3)*m(2,4)+m(4,1)*m(3,2)*m(1,4)*m(2,3));

 if (fabs(det) < 1e-10)
 {
  static MathException exception;
  exception.setErrorTypeSingularMatrix();
  throw exception;
 }
 A(1,1) = (m(2,2)*m(3,3)*m(4,4)-m(2,2)*m(3,4)*m(4,3)-m(3,2)*m(2,3)*m(4,4)+m(3,2)*m(2,4)*m(4,3)+m(4,2)*m(2,3)*m(3,4)-m(4,2)*m(2,4)*m(3,3));
 A(1,2) = -(m(1,2)*m(3,3)*m(4,4)-m(1,2)*m(3,4)*m(4,3)-m(3,2)*m(1,3)*m(4,4)+m(3,2)*m(1,4)*m(4,3)+m(4,2)*m(1,3)*m(3,4)-m(4,2)*m(1,4)*m(3,3));
 A(1,3) = (m(1,2)*m(2,3)*m(4,4)-m(1,2)*m(2,4)*m(4,3)-m(2,2)*m(1,3)*m(4,4)+m(2,2)*m(1,4)*m(4,3)+m(4,2)*m(1,3)*m(2,4)-m(4,2)*m(1,4)*m(2,3));
 A(1,4) = -(m(1,2)*m(2,3)*m(3,4)-m(1,2)*m(2,4)*m(3,3)-m(2,2)*m(1,3)*m(3,4)+m(2,2)*m(1,4)*m(3,3)+m(3,2)*m(1,3)*m(2,4)-m(3,2)*m(1,4)*m(2,3));
 A(2,1) = -(m(2,1)*m(3,3)*m(4,4)-m(2,1)*m(3,4)*m(4,3)-m(3,1)*m(2,3)*m(4,4)+m(3,1)*m(2,4)*m(4,3)+m(4,1)*m(2,3)*m(3,4)-m(4,1)*m(2,4)*m(3,3));
 A(2,2) = (m(1,1)*m(3,3)*m(4,4)-m(1,1)*m(3,4)*m(4,3)-m(3,1)*m(1,3)*m(4,4)+m(3,1)*m(1,4)*m(4,3)+m(4,1)*m(1,3)*m(3,4)-m(4,1)*m(1,4)*m(3,3));
 A(2,3) = -(m(1,1)*m(2,3)*m(4,4)-m(1,1)*m(2,4)*m(4,3)-m(2,1)*m(1,3)*m(4,4)+m(2,1)*m(1,4)*m(4,3)+m(4,1)*m(1,3)*m(2,4)-m(4,1)*m(1,4)*m(2,3));
 A(2,4) = (m(1,1)*m(2,3)*m(3,4)-m(1,1)*m(2,4)*m(3,3)-m(2,1)*m(1,3)*m(3,4)+m(2,1)*m(1,4)*m(3,3)+m(3,1)*m(1,3)*m(2,4)-m(3,1)*m(1,4)*m(2,3));
 A(3,1) = (m(2,1)*m(3,2)*m(4,4)-m(2,1)*m(3,4)*m(4,2)-m(3,1)*m(2,2)*m(4,4)+m(3,1)*m(2,4)*m(4,2)+m(4,1)*m(2,2)*m(3,4)-m(4,1)*m(2,4)*m(3,2));
 A(3,2) = -(m(1,1)*m(3,2)*m(4,4)-m(1,1)*m(3,4)*m(4,2)-m(3,1)*m(1,2)*m(4,4)+m(3,1)*m(1,4)*m(4,2)+m(4,1)*m(1,2)*m(3,4)-m(4,1)*m(1,4)*m(3,2));
 A(3,3) = (m(1,1)*m(2,2)*m(4,4)-m(1,1)*m(2,4)*m(4,2)-m(2,1)*m(1,2)*m(4,4)+m(2,1)*m(1,4)*m(4,2)+m(4,1)*m(1,2)*m(2,4)-m(4,1)*m(1,4)*m(2,2));
 A(3,4) = -(m(1,1)*m(2,2)*m(3,4)-m(1,1)*m(2,4)*m(3,2)-m(2,1)*m(1,2)*m(3,4)+m(2,1)*m(1,4)*m(3,2)+m(3,1)*m(1,2)*m(2,4)-m(3,1)*m(1,4)*m(2,2));
 A(4,1) = -(m(2,1)*m(3,2)*m(4,3)-m(2,1)*m(3,3)*m(4,2)-m(3,1)*m(2,2)*m(4,3)+m(3,1)*m(2,3)*m(4,2)+m(4,1)*m(2,2)*m(3,3)-m(4,1)*m(2,3)*m(3,2));
 A(4,2) = (m(1,1)*m(3,2)*m(4,3)-m(1,1)*m(3,3)*m(4,2)-m(3,1)*m(1,2)*m(4,3)+m(3,1)*m(1,3)*m(4,2)+m(4,1)*m(1,2)*m(3,3)-m(4,1)*m(1,3)*m(3,2));
 A(4,3) = -(m(1,1)*m(2,2)*m(4,3)-m(1,1)*m(2,3)*m(4,2)-m(2,1)*m(1,2)*m(4,3)+m(2,1)*m(1,3)*m(4,2)+m(4,1)*m(1,2)*m(2,3)-m(4,1)*m(1,3)*m(2,2));
 A(4,4) = (m(1,1)*m(2,2)*m(3,3)-m(1,1)*m(2,3)*m(3,2)-m(2,1)*m(1,2)*m(3,3)+m(2,1)*m(1,3)*m(3,2)+m(3,1)*m(1,2)*m(2,3)-m(3,1)*m(1,3)*m(2,2));
 return (A/det);
}

template<int size, class T>
Matrix<size, size, T> inverse(const Matrix<size, size, T> &matrix)
{
 if ( (size == 5) || (size == 7) )//|| (size > 8) )
 {
  static MathException exception;
  exception.setErrorTypeDimensionTooLarge();
  throw exception;
 }

 Matrix<size, size, T> invertedMatrix; 

 if( size == 1)
 {
  invertedMatrix(1,1) = 1.0/matrix(1,1);
  return invertedMatrix;
 }
   
 int subSize = (int)size/2;
 Matrix<size/2, size/2, T> A1;
 Matrix<size/2, size/2, T> A2;
 Matrix<size/2, size/2, T> A3;
 Matrix<size/2, size/2, T> A4;

 Matrix<size/2, size/2, T> iA1;
 Matrix<size/2, size/2, T> iA2;
 Matrix<size/2, size/2, T> iA3;
 Matrix<size/2, size/2, T> iA4;
	
 matrix.getSubMatrix(1,1,A1);
 matrix.getSubMatrix(1,(subSize+1), A2);
 matrix.getSubMatrix((subSize+1),1,A3);
 matrix.getSubMatrix((subSize+1),(subSize+1),A4);

 iA1 = inverse(A1-(A2*inverse(A4))*A3);
 iA2 = (-1.0)*(inverse(A1)*A2)*(inverse(A4-(A3*inverse(A1))*A2));
 iA3 = (-1.0)*(inverse(A4)*A3)*(inverse(A1-(A2*inverse(A4))*A3));
 iA4 = inverse(A4-(A3*inverse(A1))*A2);

 invertedMatrix.setSubMatrix(1,1,iA1);
 invertedMatrix.setSubMatrix(1,(subSize+1),iA2);
 invertedMatrix.setSubMatrix((subSize+1),1,iA3);
 invertedMatrix.setSubMatrix((subSize+1),(subSize+1),iA4);
 return invertedMatrix;
}

//======================================================================== 
// determinant
//========================================================================  

template<class T> 
T determinant ( Matrix <2, 2, T> &matrix)
{
 return ( matrix(1,1) * matrix(2,2) - matrix(2,1) * matrix(1,2) );
}


template<class T> 
T determinant ( Matrix <3, 3, T> &m)
{
 return (m(1,1)*m(2,2)*m(3,3)-m(1,1)*m(2,3)*m(3,2)-m(2,1)*m(1,2)*m(3,3)+m(2,1)
 	*m(1,3)*m(3,2)+m(3,1)*m(1,2)*m(2,3)-m(3,1)*m(1,3)*m(2,2));
}

template<class T> 
T determinant ( Matrix <4, 4, T> &m)
{
 return (m(1,1)*m(2,2)*m(3,3)*m(4,4)-m(1,1)*m(2,2)*m(3,4)*m(4,3)-m(1,1)*m(3,2)*m(2,3)*m(4,4)+m(1,1)*m(3,2)*m(2,4)*m(4,3)+m(1,1)
        *m(4,2)*m(2,3)*m(3,4)-m(1,1)*m(4,2)*m(2,4)*m(3,3)-m(2,1)*m(1,2)*m(3,3)*m(4,4)+m(2,1)*m(1,2)*m(3,4)*m(4,3)+m(2,1)*m(3,2)
	*m(1,3)*m(4,4)-m(2,1)*m(3,2)*m(1,4)*m(4,3)-m(2,1)*m(4,2)*m(1,3)*m(3,4)+m(2,1)*m(4,2)*m(1,4)*m(3,3)+m(3,1)*m(1,2)*m(2,3)
	*m(4,4)-m(3,1)*m(1,2)*m(2,4)*m(4,3)-m(3,1)*m(2,2)*m(1,3)*m(4,4)+m(3,1)*m(2,2)*m(1,4)*m(4,3)+m(3,1)*m(4,2)*m(1,3)*m(2,4)
	-m(3,1)*m(4,2)*m(1,4)*m(2,3)-m(4,1)*m(1,2)*m(2,3)*m(3,4)+m(4,1)*m(1,2)*m(2,4)*m(3,3)+m(4,1)*m(2,2)*m(1,3)*m(3,4)-m(4,1)
	*m(2,2)*m(1,4)*m(3,3)-m(4,1)*m(3,2)*m(1,3)*m(2,4)+m(4,1)*m(3,2)*m(1,4)*m(2,3));
}


template<class T> 
T determinant ( Matrix <5, 5, T> &m)
{
	return (
m(4,1)*m(3,2)*m(2,3)*m(1,4)*m(5,5)-m(2,1)*m(1,2)*m(3,3)*m(4,4)*m(5,5)+
m(2,1)*m(1,2)*m(3,3)*m(4,5)*m(5,4)+m(2,1)*m(1,2)*m(4,3)*m(3,4)*m(5,5)-
m(2,1)*m(1,2)*m(4,3)*m(3,5)*m(5,4)-m(2,1)*m(1,2)*m(5,3)*m(3,4)*m(4,5)+
m(2,1)*m(1,2)*m(5,3)*m(3,5)*m(4,4)+m(2,1)*m(3,2)*m(1,3)*m(4,4)*m(5,5)-
m(2,1)*m(3,2)*m(1,3)*m(4,5)*m(5,4)-m(2,1)*m(3,2)*m(4,3)*m(1,4)*m(5,5)+
m(2,1)*m(3,2)*m(4,3)*m(1,5)*m(5,4)+m(2,1)*m(3,2)*m(5,3)*m(1,4)*m(4,5)-
m(2,1)*m(3,2)*m(5,3)*m(1,5)*m(4,4)-m(2,1)*m(4,2)*m(1,3)*m(3,4)*m(5,5)+
m(2,1)*m(4,2)*m(1,3)*m(3,5)*m(5,4)+m(2,1)*m(4,2)*m(3,3)*m(1,4)*m(5,5)-
m(2,1)*m(4,2)*m(3,3)*m(1,5)*m(5,4)-m(2,1)*m(4,2)*m(5,3)*m(1,4)*m(3,5)+
m(2,1)*m(4,2)*m(5,3)*m(1,5)*m(3,4)+m(2,1)*m(5,2)*m(1,3)*m(3,4)*m(4,5)-
m(2,1)*m(5,2)*m(1,3)*m(3,5)*m(4,4)-m(2,1)*m(5,2)*m(3,3)*m(1,4)*m(4,5)+
m(2,1)*m(5,2)*m(3,3)*m(1,5)*m(4,4)+m(2,1)*m(5,2)*m(4,3)*m(1,4)*m(3,5)-
m(2,1)*m(5,2)*m(4,3)*m(1,5)*m(3,4)+m(3,1)*m(1,2)*m(2,3)*m(4,4)*m(5,5)-
m(3,1)*m(1,2)*m(2,3)*m(4,5)*m(5,4)-m(3,1)*m(1,2)*m(4,3)*m(2,4)*m(5,5)+
m(3,1)*m(1,2)*m(4,3)*m(2,5)*m(5,4)+m(3,1)*m(1,2)*m(5,3)*m(2,4)*m(4,5)-
m(3,1)*m(1,2)*m(5,3)*m(2,5)*m(4,4)-m(3,1)*m(2,2)*m(1,3)*m(4,4)*m(5,5)+
m(3,1)*m(2,2)*m(1,3)*m(4,5)*m(5,4)+m(3,1)*m(2,2)*m(4,3)*m(1,4)*m(5,5)-
m(3,1)*m(2,2)*m(4,3)*m(1,5)*m(5,4)-m(3,1)*m(2,2)*m(5,3)*m(1,4)*m(4,5)+
m(3,1)*m(2,2)*m(5,3)*m(1,5)*m(4,4)+m(3,1)*m(4,2)*m(1,3)*m(2,4)*m(5,5)-
m(3,1)*m(4,2)*m(1,3)*m(2,5)*m(5,4)-m(3,1)*m(4,2)*m(2,3)*m(1,4)*m(5,5)+
m(3,1)*m(4,2)*m(2,3)*m(1,5)*m(5,4)+m(3,1)*m(4,2)*m(5,3)*m(1,4)*m(2,5)-
m(3,1)*m(4,2)*m(5,3)*m(1,5)*m(2,4)-m(3,1)*m(5,2)*m(1,3)*m(2,4)*m(4,5)+
m(3,1)*m(5,2)*m(1,3)*m(2,5)*m(4,4)+m(3,1)*m(5,2)*m(2,3)*m(1,4)*m(4,5)-
m(3,1)*m(5,2)*m(2,3)*m(1,5)*m(4,4)-m(3,1)*m(5,2)*m(4,3)*m(1,4)*m(2,5)+
m(3,1)*m(5,2)*m(4,3)*m(1,5)*m(2,4)-m(4,1)*m(1,2)*m(2,3)*m(3,4)*m(5,5)+
m(4,1)*m(1,2)*m(2,3)*m(3,5)*m(5,4)+m(4,1)*m(1,2)*m(3,3)*m(2,4)*m(5,5)-
m(4,1)*m(1,2)*m(3,3)*m(2,5)*m(5,4)-m(4,1)*m(1,2)*m(5,3)*m(2,4)*m(3,5)+
m(4,1)*m(1,2)*m(5,3)*m(2,5)*m(3,4)+m(4,1)*m(2,2)*m(1,3)*m(3,4)*m(5,5)-
m(4,1)*m(2,2)*m(1,3)*m(3,5)*m(5,4)-m(4,1)*m(2,2)*m(3,3)*m(1,4)*m(5,5)+
m(4,1)*m(2,2)*m(3,3)*m(1,5)*m(5,4)+m(4,1)*m(2,2)*m(5,3)*m(1,4)*m(3,5)-
m(4,1)*m(2,2)*m(5,3)*m(1,5)*m(3,4)-m(4,1)*m(3,2)*m(1,3)*m(2,4)*m(5,5)+
m(4,1)*m(3,2)*m(1,3)*m(2,5)*m(5,4)-m(4,1)*m(3,2)*m(2,3)*m(1,5)*m(5,4)-
m(4,1)*m(3,2)*m(5,3)*m(1,4)*m(2,5)+m(4,1)*m(3,2)*m(5,3)*m(1,5)*m(2,4)+
m(4,1)*m(5,2)*m(1,3)*m(2,4)*m(3,5)-m(4,1)*m(5,2)*m(1,3)*m(2,5)*m(3,4)-
m(4,1)*m(5,2)*m(2,3)*m(1,4)*m(3,5)+m(4,1)*m(5,2)*m(2,3)*m(1,5)*m(3,4)+
m(4,1)*m(5,2)*m(3,3)*m(1,4)*m(2,5)-m(4,1)*m(5,2)*m(3,3)*m(1,5)*m(2,4)+
m(5,1)*m(1,2)*m(2,3)*m(3,4)*m(4,5)-m(5,1)*m(1,2)*m(2,3)*m(3,5)*m(4,4)-
m(5,1)*m(1,2)*m(3,3)*m(2,4)*m(4,5)+m(5,1)*m(1,2)*m(3,3)*m(2,5)*m(4,4)+
m(5,1)*m(1,2)*m(4,3)*m(2,4)*m(3,5)-m(5,1)*m(1,2)*m(4,3)*m(2,5)*m(3,4)-
m(5,1)*m(2,2)*m(1,3)*m(3,4)*m(4,5)+m(5,1)*m(2,2)*m(1,3)*m(3,5)*m(4,4)+
m(5,1)*m(2,2)*m(3,3)*m(1,4)*m(4,5)-m(5,1)*m(2,2)*m(3,3)*m(1,5)*m(4,4)-
m(5,1)*m(2,2)*m(4,3)*m(1,4)*m(3,5)+m(5,1)*m(2,2)*m(4,3)*m(1,5)*m(3,4)+
m(5,1)*m(3,2)*m(1,3)*m(2,4)*m(4,5)-m(5,1)*m(3,2)*m(1,3)*m(2,5)*m(4,4)-
m(5,1)*m(3,2)*m(2,3)*m(1,4)*m(4,5)+m(5,1)*m(3,2)*m(2,3)*m(1,5)*m(4,4)+
m(5,1)*m(3,2)*m(4,3)*m(1,4)*m(2,5)-m(5,1)*m(3,2)*m(4,3)*m(1,5)*m(2,4)-
m(5,1)*m(4,2)*m(1,3)*m(2,4)*m(3,5)+m(5,1)*m(4,2)*m(1,3)*m(2,5)*m(3,4)+
m(5,1)*m(4,2)*m(2,3)*m(1,4)*m(3,5)-m(5,1)*m(4,2)*m(2,3)*m(1,5)*m(3,4)-
m(5,1)*m(4,2)*m(3,3)*m(1,4)*m(2,5)+m(5,1)*m(4,2)*m(3,3)*m(1,5)*m(2,4)+
(m(2,2)*m(3,3)*m(4,4)*m(5,5)-m(2,2)*m(3,3)*m(4,5)*m(5,4)-m(2,2)*m(4,3)*m(3,4)*m(5,5)+
 m(2,2)*m(4,3)*m(3,5)*m(5,4)+m(2,2)*m(5,3)*m(3,4)*m(4,5)-m(2,2)*m(5,3)*m(3,5)*m(4,4)-
 m(3,2)*m(2,3)*m(4,4)*m(5,5)+m(3,2)*m(2,3)*m(4,5)*m(5,4)+m(3,2)*m(4,3)*m(2,4)*m(5,5)-
 m(3,2)*m(4,3)*m(2,5)*m(5,4)-m(3,2)*m(5,3)*m(2,4)*m(4,5)+m(3,2)*m(5,3)*m(2,5)*m(4,4)+
 m(4,2)*m(2,3)*m(3,4)*m(5,5)-m(4,2)*m(2,3)*m(3,5)*m(5,4)-m(4,2)*m(3,3)*m(2,4)*m(5,5)+
 m(4,2)*m(3,3)*m(2,5)*m(5,4)+m(4,2)*m(5,3)*m(2,4)*m(3,5)-m(4,2)*m(5,3)*m(2,5)*m(3,4)-
 m(5,2)*m(2,3)*m(3,4)*m(4,5)+m(5,2)*m(2,3)*m(3,5)*m(4,4)+m(5,2)*m(3,3)*m(2,4)*m(4,5)-
 m(5,2)*m(3,3)*m(2,5)*m(4,4)-m(5,2)*m(4,3)*m(2,4)*m(3,5)+m(5,2)*m(4,3)*m(2,5)*m(3,4))*m(1,1));
}


template<int size, class T> 
T determinant ( Matrix <size, size, T> &m)
{
 Matrix<size-1, size-1, T> sm;
 T det;
 int coeff;
 int i, r, c;
 
 det = 0;
 for ( i = 1; i <= size; i++ )
 {
  for ( r = 1; r <= size; r++ )
  {
   for ( c = 2; c <= size; c++)
   {
    if( r != i )
    {
     if( r < i )
      sm(r, c-1) = m(r, c);
     else
      sm(r-1, c-1)  =  m(r, c);
    }
   }
  }
  (i%2 == 0 ) ? (coeff = -1) : (coeff = 1);
  det += coeff * m(i, 1) * determinant(sm);
 } 									
 return det;
}


//======================================================================== 
// trace
//========================================================================  
template<int size, class T> 
T trace ( Matrix <size, size, T> &m)
{
 T tr;
 tr = 0;
 for (int i = 1; i <= size; i++)
  tr = tr + m(i, i);
 return tr;
}


#endif
