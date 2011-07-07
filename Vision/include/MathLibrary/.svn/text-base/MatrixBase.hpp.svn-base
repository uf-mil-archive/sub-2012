//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: MatrixBase.hpp
// Interface of the class MatrixBase.
//========================================================================  

#ifndef INCLUDED_MatrixBase_hpp
#define INCLUDED_MatrixBase_hpp 

#include "MathException.hpp"
#include "MatrixInitializer.hpp"

//====================================================================
// class MatrixBase
// -------------------------------------------------------------------
// \brief
// This is a pure virtual base class for \c Matrix.
//====================================================================

template<class T = double>
class MatrixBase
{
 public:
  MatrixBase() {}
   // The default constructor.

  virtual ~MatrixBase() {};
   // The default destructor.

  virtual T *getElementsPointer() const = 0;
   //  return  The pointer to the first element in a matrix or vector.
		
  virtual int getNumRows() const = 0;
   //  return  The number of rows in the Matrix.

  virtual int getNumColumns() const = 0;
   //  return  The number of columns in the Matrix.

  virtual T getElement(int row, int column) const = 0;
   //  return  The element at the specified position.

  virtual void setElement(int row, int column, T value)= 0;
   // Sets an element to a value at the specified position.
   //  row	Row number of the desired element.
   //  column	Column number of the desired element.
   //  value	The desired element is set to this value.

  MatrixBase<T> &operator=(const MatrixBase<T> &matrixBase);
   // Assignment operator between two \c MatrixBase types of 
   // same dimensions.
	
  virtual MatrixInitializer<T> operator=(const T &value) = 0;
   // Initialization of matrix.
   //  value  The value to which all elements in the matrix
   //         are initialized.
	
  void output(ostream &outputStream = cout);
   //  return  The elements in the referenced matrix to 
   //          the output stream (by default the
   //          output is to the console)
		
 protected:
};

// ========== END OF INTERFACE ==========

//=======================================================================
// MatrixBase::operator=
//=======================================================================
template<class T>
MatrixBase<T> &MatrixBase<T>::operator=(const MatrixBase<T> &matrixBase)
{
 int numRows = getNumRows();
 int numColumns = getNumColumns();
 
 if ( (numRows != matrixBase.getNumRows()) || (numColumns != matrixBase.getNumColumns()) )
 {
  static MathException exception;
  exception.setErrorTypeIncompatibleSize();
  throw exception;
 }
 
 for (int r = 1; r <= numRows; r++)
 {
  for (int c = 1; c <= numColumns; c++)
  {
   setElement(r, c, matrixBase.getElement(r, c));
  }
 }
 return *this;
}


//=======================================================================
// MatrixBase::output
//=======================================================================
template<class T>
void MatrixBase<T>::output(ostream &outputStream)
{
 int numRows = getNumRows();
 int numColumns = getNumColumns();
	
 for (int r = 1; r <= numRows; r++)
 {
  for (int c = 1; c <= numColumns; c++)
  {
   outputStream << getElement(r, c) << " ";
  }
  if (r < numRows) outputStream << "\n";
 }
}


#endif
