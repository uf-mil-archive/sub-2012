//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: VectorGSL.hpp
// Interface of the class VectorGSL.
//========================================================================  
 

#ifndef INCLUDED_VectorGSL_hpp
#define INCLUDED_VectorGSL_hpp
 
#include "ColumnVector.hpp" 
 
//======================================================================== 
// class VectorGSL
// ----------------------------------------------------------------------
// \brief
// The class \c VectorGSL provides is equivalent to a \c ColumnVector object.
//
// <b>Example Program:</b> See the Example program for class ColumnVector.
//========================================================================  
 
template<int size, class T> class ColumnVector;

template<int size, class T = double>
class VectorGSL : public ColumnVector<size, T>
{
 public:
  inline VectorGSL () {}
   // The default constructor. No element initializations.

  inline VectorGSL(const VectorGSL<size, T> &vector);
   // Copy Constructor. 

  ~VectorGSL () {}
   // The default Destructor.
		
  inline VectorGSL(const Matrix<size,1,T> &matrix);
   // The conversion constructor for conversion
   // of a \c Matrix type of single column into
   // type \c VectorGSL.
			 	
  inline VectorGSL<size, T> &operator=(const VectorBase<T> &vectorBase);
   // Assign a \c VectorBase type to a \c VectorGSL type. Both objects 
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

//=============================================
// Declaration of Template functions.
//=============================================

template<class T>
VectorGSL<3,T> crossProduct(const VectorGSL<3,T> &v1, const VectorGSL<3,T> &v2);
 // Generates the cross product of two 3 dimensional vectors. 
 //  v1, v2  The 3D vector arguments.
 //  return  The cross product.


template<int size, class T>
VectorGSL<size, T> elementProduct(const VectorGSL<size,T> &v1, const VectorGSL<size,T> &v2);
 // This function performs multiplication between two vectors 
 // element-by-element.
 //  v1, v2  The vector arguments
 //  return  The product.

template<int size, class T>
T dotProduct(const VectorGSL<size,T> &v1, const VectorGSL<size,T> &v2);
 // Dot (inner) product between two vectors.
 //  v1, v2 The vector arguments.
 //  return  The scalar product.

template<int size, class T>
inline RowVector<size, T> transpose(const VectorGSL<size, T> &vector);
 //  return  The transpose of type \c RowVector.


// ========== END OF INTERFACE ==========


//======================================================================== 
// VectorGSL::VectorGSL
//========================================================================  
template<int size, class T>
VectorGSL<size, T>::VectorGSL(const VectorGSL<size, T> &v)
{
 for (int r = 0; r < size; r++)
  this->d_element[r][0] = v.d_element[r][0];
}

template<int size, class T>
VectorGSL<size, T>::VectorGSL(const Matrix<size,1,T> &m)
{
 for (int r = 1; r <= size; r++)
  this->d_element[r-1][0] = m(r, 1);
}


//======================================================================== 
// VectorGSL::operator=
//========================================================================  
template<int size, class T>
VectorGSL<size, T> &VectorGSL<size, T>::operator=(const VectorBase<T> &v)
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
MatrixInitializer<T>VectorGSL<size, T>::operator=(const T &value)
{
 for (int i = 1; i <= size; i++)
 {
  this->d_element[i-1][0] = value;
 }
 MatrixInitializer<T>  matrixInitialize(size, 1, *this->d_element);
 return matrixInitialize;
}


//======================================================================== 
// crossProduct
//========================================================================  
template<class T>
VectorGSL<3,T> crossProduct(const VectorGSL<3,T> &v1, const VectorGSL<3,T> &v2)
{
 VectorGSL<3, T> cp;
 cp(1) = v1(2)*v2(3) - v1(3)*v2(2);
 cp(2) = v1(3)*v2(1) - v1(1)*v2(3);
 cp(3) = v1(1)*v2(2) - v1(2)*v2(1);
 return (cp);
}


//======================================================================== 
// elementProduct
//========================================================================  
template<int size, class T>
VectorGSL<size, T> elementProduct(const VectorGSL<size,T> &v1, const VectorGSL<size,T> &v2)
{
 VectorGSL<size, T> ep;
 for (int i = 1; i <= size; i++)
  ep(i) = v1(i)*v2(i);
 return (ep);
}


//======================================================================== 
// dotProduct
//========================================================================  
template<int size, class T>
T dotProduct(const VectorGSL<size,T> &v1, const VectorGSL<size,T> &v2)
{
 T d = 0;
 for (int i = 1; i <= size; i++)
  d = d + v1(i) * v2(i);
 return (d);
}


//======================================================================== 
// transpose
//========================================================================  
template<int size, class T>
RowVector<size, T> transpose(const VectorGSL<size, T> &v)
{
 RowVector<size,T> row;
 for (int i = 1; i <= size; i++)
  row(i) = v(i);
 return row;
}



#endif

