//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: ColumnVector.hpp
// Interface of the class ColumnVector.
//========================================================================  
 

#ifndef INCLUDED_ColumnVector_hpp
#define INCLUDED_ColumnVector_hpp

#include "Matrix.hpp"
#include "VectorBase.hpp"


//====================================================================
// class ColumnVector
// -------------------------------------------------------------------
// \brief
// A class for column vectors.
//
// The class \c ColumnVector is derived from the base classes \c Matrix
// and \c VectorBase, and provides methods for operations such as 
// cross product, dot product and element-by-element multiplication.
//
// <b>Example Program:</b>
// \include VectorGSL.t.cpp
//========================================================================  
 
template<int size, class T> class VectorGSL;

template<int size, class T = double>
class ColumnVector : public Matrix<size, 1, T>, public VectorBase<T>
{
 public:
  inline ColumnVector() {}
   // The default constructor. The elements are not initialized.
	
  inline ColumnVector(const ColumnVector<size, T> &columnVector);
   // Copy Constructor. 

  inline ColumnVector(const Matrix<size,1,T> &matrix);
   // The conversion constructor for conversion
   // of a \c Matrix type of single column into
   // type \c ColumnVector.
		
  inline ColumnVector(const VectorGSL<size,T> &vector);
   // The conversion constructor for conversion
   // of a \c VectorGSL type into \c ColumnVector.
		
  virtual ~ColumnVector(){}
   // The default destructor.
	
  virtual T *getElementsPointer() const { return (T *) &(this->d_element[0][0]); }
   //  return A pointer to the first element in the vector.

  inline virtual T getElement(int index) const;
   //  return  The value at position specified by index 
   //          (index = 1 is the first element).

  inline virtual void setElement(int index, T value);
   // Sets an element to a value at the specified position.
   //  index  Position of the desired element.
   //  value  The desired element is set to this value.

  virtual bool isRowVector() const {return false;}
   //  return  false

  virtual int getNumElements() const {return size;}
   //  return The number of elements in the vector.

  inline T &operator()(int index);
   // Access or assign the element at the position specified by 
   // index. For example: 
   // \code 
   // myVector(2)=12.65; 
   // \endcode
	
  inline T operator()(int index) const;
   // Access the element at the position specified by  
   // index.
  
  inline ColumnVector<size, T> &operator=(const VectorBase<T> &vectorBase);
   // Assign a \c VectorBase type to a \c ColumnVector type. Both objects 
   // must have the same dimensions.

  MatrixInitializer<T> operator=(const T &value);
   // Initialize a vector object.
   //  value  The value to which all elements in the vector are initialized.
   // The initialization of the vector object can also
   // be done as a comma seperated list. For example:
   // \code 
   // ColumnVector<3> myVector;
   // myVector = 67.88, 45.89, 90; 
   // \endcode
 private:
};


template<class T>
ColumnVector<3,T> crossProduct(const ColumnVector<3,T> &v1, const ColumnVector<3,T> &v2);
 // Generates the cross product of two 3 dimensional column vectors. 
 //  v1, v2  The 3D column-vector arguments.
 //  return  The cross product.

template<int size, class T>
ColumnVector<size, T> elementProduct(const ColumnVector<size,T> &v1, const ColumnVector<size,T> &v2);
 // This function performs multiplication between two column vectors 
 // element-by-element.
 //  v1, v2  The column-vector arguments
 //  return  The product.

template<int size, class T>
T dotProduct(const ColumnVector<size,T> &v1, const ColumnVector<size,T> &v2);
 // Dot (inner) product between two column-vectors.
 //  v1, v2 The column-vector arguments.
 //  return  The scalar product.

template<int size, class T>
inline RowVector<size, T> transpose(const ColumnVector<size, T> &vector);
 //  return  The transpose of type \c RowVector.

// ========== END OF INTERFACE ==========
	


//==============================================================
// Definition of inlined functions.
//==============================================================

//======================================================================== 
// ColumnVector::ColumnVector: Constructor of the ColumnVector class
//======================================================================== 
template<int size, class T>
ColumnVector<size, T>::ColumnVector(const Matrix<size,1,T> &m)
{
 for (int r = 1; r <= size; r++)
  this->d_element[r-1][0] = m(r, 1);
}

template<int size, class T>
ColumnVector<size, T>::ColumnVector(const ColumnVector<size, T> &v)
{
 for (int r = 0; r < size; r++)
  this->d_element[r][0] = v.d_element[r][0];
}
	
template<int size, class T>
ColumnVector<size, T>::ColumnVector(const VectorGSL<size, T> &v)
{
 for (int r = 1; r <= size; r++)
  this->d_element[r-1][0] = v(r);
}


//======================================================================== 
// ColumnVector::getElement
//========================================================================  
template<int size, class T>
T ColumnVector<size, T>::getElement(int i) const
{
 if( i > size || i < 1)
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 return this->d_element[(i-1)][0];
}


//======================================================================== 
// ColumnVector::setElement
//========================================================================  
template<int size, class T>
void ColumnVector<size, T>::setElement(int i, T value)
{
 if( i > size || i < 1)
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 this->d_element[(i-1)][0] = value;
}


//======================================================================== 
// ColumnVector::operator()
//========================================================================  
template<int size, class T>
T &ColumnVector<size, T>::operator()(int i)
{
 if( i > size || i < 1)
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 return this->d_element[(i-1)][0];
}


template<int size, class T>
T ColumnVector<size, T>::operator()(int i) const 
{
 if( i > size || i < 1)
 {
  static MathException exception;
  exception.setErrorTypeIndexOutOfRange();
  throw exception;
 }
 return this->d_element[(i-1)][0];
}


//======================================================================== 
// crossProduct
//========================================================================  
template<class T>
ColumnVector<3,T> crossProduct(const ColumnVector<3,T> &v1, const ColumnVector<3,T> &v2)
{
 ColumnVector<3, T> cp;
 cp(1) = v1(2)*v2(3) - v1(3)*v2(2);
 cp(2) = v1(3)*v2(1) - v1(1)*v2(3);
 cp(3) = v1(1)*v2(2) - v1(2)*v2(1);
 return (cp);
}


//======================================================================== 
// elementProduct
//========================================================================  
template<int size, class T>
ColumnVector<size, T> elementProduct(const ColumnVector<size,T> &v1, const ColumnVector<size,T> &v2)
{
 ColumnVector<size, T> ep;
 for (int i = 1; i <= size; i++)
  ep(i) = v1(i)*v2(i);
 return (ep);
}


//======================================================================== 
// dotProduct
//========================================================================  
template<int size, class T>
T dotProduct(const ColumnVector<size,T> &v1, const ColumnVector<size,T> &v2)
{
 T d = 0;
 for (int i = 1; i <= size; i++)
  d = d + v1(i)*v2(i);
 return (d);
}


//======================================================================== 
// Transpose
//========================================================================  
template<int size, class T>
RowVector<size, T> transpose(const ColumnVector<size, T> &v)
{
 RowVector<size,T> row;
 for (int i = 1; i <= size; i++)
  row(i) = v(i);
 return row;
}


//======================================================================== 
// ColumnVector::operator=
//========================================================================  
template<int size, class T>
ColumnVector<size, T> &ColumnVector<size, T>::operator=(const VectorBase<T> &v)
{
 if(!v.isColumnVector()) 
 {
  static MathException exception;
  exception.setErrorTypeTypeMismatch();
  throw exception;
 }

 if (size != v.getNumElements())
 {
  static MathException exception;
  exception.setErrorTypeIncompatibleSize();
  throw exception;
 }
 
 for (int i = 1; i <= size; i++)
 {
  this->d_element[i-1][0] = v.getElement(i);
 }
 return *this;
}


template<int size, class T>
MatrixInitializer<T> ColumnVector<size, T>::operator=(const T &value)
{
 for (int i = 1; i <= size; i++)
 {
  this->d_element[i-1][0] = value;
 }
 MatrixInitializer<T>  matrixInitialize(size, 1, *this->d_element);
 return matrixInitialize;
}


#endif





