//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: VectorBase.hpp
// Interface of the class VectorBase.
//========================================================================  
 
#ifndef INCLUDED_VectorBase_hpp
#define INCLUDED_VectorBase_hpp

#include "MathException.hpp"
#include "MatrixInitializer.hpp"

//====================================================================
// class VectorBase
// -------------------------------------------------------------------
// \brief
// The pure virtual base class for \c ColumnVector, \c RowVector and 
// \c Vector classes.
//========================================================================  
 
template<class T> class MatrixBase;

template<class T = double>
class VectorBase
{
 public:
  VectorBase(){}
   // The default constructor.

  virtual ~VectorBase(){}
   // The default destructor.

  virtual T *getElementsPointer() const = 0;
   //  return  A pointer to the first element in a vector.

  virtual T getElement(int i) const = 0;
   //  return  The element at the index i.

  virtual void setElement(int index, T value)= 0;
   // Sets an element to a value at the specified position.
   //  index  Position of the desired element.
   //  value  The desired element is set to this value.
	
  virtual int getNumElements() const  = 0;
   //  return  The number of elements in the vector.

  inline T norm() const;
   //  return 2-norm of the vector.
	
  virtual bool isRowVector() const = 0;
   //  return  TRUE if the vector instantiated is a \c RowVector.

  bool isColumnVector() const {return !isRowVector(); }
   //  return  TRUE if the vector instantiated is a \c ColumnVector.

  VectorBase<T> &operator=(const VectorBase<T> &vectorBase);
   // Assignment operator between two VectorBase types.
   
  virtual MatrixInitializer<T> operator=(const T &value) = 0;
   // Initialize a vector object.
	
  void output(ostream &outputStream = cout);
   //  return  The elements in the vector to the output stream 
   //          (by default the output is to the console)
		
private:
};

// ========== END OF INTERFACE ==========	

//=================================================================
// End of declarations.
//=================================================================

//======================================================================== 
// VectorBase::norm
//========================================================================  
template<class T>
T VectorBase<T>::norm() const
{
 T norm2;
 norm2 = 0;
 T val;
 for (int position = 1; position <= getNumElements(); position++)
 {
  val = getElement(position);
  norm2 = norm2 + (val * val);
 }
 return sqrt(norm2);
}


//=======================================================================
// VectorBase::operator=
//=======================================================================
template<class T>
VectorBase<T> &VectorBase<T>::operator=(const VectorBase<T> &v)
{
 if( v.isColumnVector() != isColumnVector() ) 
 {
  static MathException exception;
  exception.setErrorTypeTypeMismatch();
  throw exception;
 }

 int numElements = v.getNumElements();
 if (numElements != getNumElements())
 {
  static MathException exception;
  exception.setErrorTypeIncompatibleSize();
  throw exception;
 }
 
 for (int i = 1; i <= numElements; i++)
 { 
  setElement(i, v.getElement(i));
 }
 return *this;
}


//=======================================================================
// VectorBase::output
//=======================================================================
template<class T>
void VectorBase<T>::output(ostream &outputStream)
{
 int numElements = getNumElements();
	
 for (int position = 1; position <= numElements; position++)
 {
  outputStream << getElement(position);
  if (isRowVector()) outputStream << " ";
  else if(position < numElements) outputStream << "\n";
 }
}


#endif

