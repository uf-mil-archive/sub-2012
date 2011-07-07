//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: MathException.hpp
// Interface of the class MathException.
//========================================================================  
 

#ifndef INCLUDED_MathException_hpp
#define INCLUDED_MathException_hpp
 
#include <iostream>

using namespace std;

template<class T> class MatrixBase;
template<class T> class VectorBase;

 
//======================================================================== 
// class MathException
// ----------------------------------------------------------------------
// \brief
// Run-time exception handling for the math library.
//
// <b>Example Program:</b>
// \include MathException.t.cpp
//========================================================================  
	
class MathException
{
 public:
  inline MathException() {}	
   // The default constructor.

  ~MathException(){}
   // The Default Destructor
  
  inline char *getErrorMessage(); 
   // return The exception error message string.
	
  inline bool isErrorTypeIndexOutOfRange();
   //  return  true if exception is caused by an out of range index.

  inline bool isErrorTypeSingularMatrix();
   //  return  true if exception is caused by a matrix of determinant zero.

  inline bool isErrorTypeDivideByZero();
   //  return  true if exception is caused by a division by zero.

  inline bool isErrorTypeIncompatibleSize();
   //  return true if exception is caused by operation between 
   //         two non-conformable matrices/vectors.
	
  inline bool isErrorTypeTypeMismatch();
   //  return  true if exception is caused by operation 
   //          between incompatible types.

  inline bool isErrorTypeDimensionTooLarge();
   //  return  true if exception is caused by a matrix of dimensions
   //          too large for the math library to handle. (For example:
   //          calling inverse() on matrices larger than 8 x 8.)

   // ========== END OF INTERFACE ==========
	
  inline void setErrorTypeIndexOutOfRange();
   // Sets the exception type to \a e_exIndexOutOfRange.
	
  inline void setErrorTypeSingularMatrix();
   // Sets the exception type to \a e_exSingularMatrix.

  inline void setErrorTypeDivideByZero();
   // Sets the exception type to \a e_exDivideByZero.

  inline void setErrorTypeIncompatibleSize();
   // Sets the exception type to \a e_exIncompatibleMatrix.
	
  inline void setErrorTypeTypeMismatch();
   // Sets the exception type to \a e_exTypeMismatch.

  inline void setErrorTypeDimensionTooLarge();
   // Sets the exception type to \a e_exDimensionTooLarge.

  private:
  //------- Error types--------------------
  typedef enum 
  { 
   e_exIndexOutOfRange = 100, 
   e_exSingularMatrix, 
   e_exDivideByZero,
   e_exIncompatibleSize, 
   e_exTypeMismatch,
   e_exDimensionTooLarge
  }exceptionType;
  exceptionType d_errorType;
};

//==========================================================
// End of declarations.
//==========================================================


//==========================================================
// MathException::isErrorType.....
//==========================================================
bool MathException::isErrorTypeIndexOutOfRange()
{
 if (d_errorType == e_exIndexOutOfRange)
  return true;
 return false;
}

bool MathException::isErrorTypeSingularMatrix()
{
 if (d_errorType == e_exSingularMatrix)
  return true;
 return false;
}

bool MathException::isErrorTypeDivideByZero()
{
 if (d_errorType == e_exDivideByZero)
  return true;
 return false;
}

bool MathException::isErrorTypeIncompatibleSize()
{
 if (d_errorType == e_exIncompatibleSize)
  return true;
 return false;
}

bool MathException::isErrorTypeTypeMismatch()
{
 if (d_errorType == e_exTypeMismatch)
  return true;
 return false;
}

bool MathException::isErrorTypeDimensionTooLarge()
{
 if (d_errorType == e_exDimensionTooLarge)
  return true;
 return false;
}

//==========================================================
// MathException::getErrorMessage
//==========================================================
char *MathException::getErrorMessage()
{
 switch(d_errorType)
 {
  case 100: return("Math Exception : Index out of range."); break;
  case 101: return("Math Exception : Singular Matrix."); break;
  case 102: return("Math Exception : Division by zero."); break;
  case 103: return("Math Exception : Operation between objects of incompatible sizes."); break;
  case 104: return("Math Exception : Operation between objects of incompatible types."); break;
  case 105: return("Math Exception : Matrix too large."); break;
  default: return("Math Exception : Math Exception has occurred.");
 }
}


//==========================================================
// MathException::setErrorType.....
//==========================================================
void MathException::setErrorTypeIndexOutOfRange()
{
 d_errorType = e_exIndexOutOfRange;
}

void MathException::setErrorTypeSingularMatrix()
{
 d_errorType = e_exSingularMatrix;
}


void MathException::setErrorTypeDivideByZero()
{
 d_errorType = e_exDivideByZero;
}


void MathException::setErrorTypeIncompatibleSize()
{
 d_errorType = e_exIncompatibleSize;
}

void MathException::setErrorTypeTypeMismatch()
{
 d_errorType = e_exTypeMismatch;
}

void MathException::setErrorTypeDimensionTooLarge()
{
 d_errorType = e_exDimensionTooLarge;
}
#endif

