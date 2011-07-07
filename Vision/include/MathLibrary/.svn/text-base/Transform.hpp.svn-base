//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: Transform.hpp
// Interface of the class Transform.
//========================================================================  
 
#ifndef INCLUDED_Transform_hpp
#define INCLUDED_Transform_hpp

#include "Matrix.hpp"
#include "ColumnVector.hpp"
#include "RowVector.hpp"

//====================================================================
// class Transform
// -------------------------------------------------------------------
// \brief
// The class \c Transform represents a 4x4 transformation matrix.
//
// The class \c Transform is derived from it's base class \c Matrix.
// The methods provided in the class \c Transform include homogeneous
// transformations about XYZ or any arbitrary axes, extraction of 
// position and orientation from the transition matrix.
// 
// <b>Example Program:</b>
// \include Transform.t.cpp
//========================================================================  
 
class Transform : public Matrix <4, 4, double>
{
 public:
  inline Transform();
   // The default constructor for the
   // \c Transform object. The \c Transform
   // matrix is initialized to the following form.
   // \code
   //	[1 0 0 0]
   //	[0 1 0 0]
   //	[0 0 1 0]
   //	[0 0 0 1]
   // \endcode

  inline Transform(const Transform &transform);
   // Copy Constructor.

  ~Transform() {}
   // The default destructor.
	
  inline MatrixInitializer<double> operator=(const double &value);
   // This function provides an overloaded assignment
   // operator for initializing the elements of a Transform.
   // The initialization of the Transform object can be 
   // done as a comma seperated list. For example:
   // \code Transform myTransform;
   // myTransform = cos(x), sin(x),...so on ; \endcode
   //  value  The value to which the element at a position 
   //         in Transform is initialized.
		
  inline ColumnVector<3,double> getTranslation() const;
   // returns the XYZ position
   // from the transition matrix.
		
  inline void getRollPitchYaw(double &roll, double &pitch, double &yaw) const;
   // returns the roll, pitch and yaw angles from the transition matrix.
   // (The rotational part of the Transform matrix is obtained by 
   // first defining a rotation about X axis by \c yaw radians, 
   // then a rotation about the Y axis by \c pitch radians and 
   // finally a rotation about the Z axis by \c roll radians, 
   // all rotations being relative to a fixed XYZ frame.)
   //  roll	The roll angle is extracted into this variable.
   //  pitch	The pitch angle is extracted into this variable.
   //  yaw	The yaw angle is extracted into this variable. 
		
  inline void getEulerAngles(double &theta, double &phi, double &xi) const;
   // returns the euler angles from the transition matrix.
   // (Besides the roll/pitch/yaw notation, the rotational part 
   // of the Transform matrix is also obtained by first 
   // defining a rotation about Z axis by \c phi radians, 
   // then a rotation about the current Y axis by \c theta 
   // radians and finally a rotation about the current Z 
   // axis by \c xi radians.)
   //  theta	The angle theta is extracted into this variable.
   //  phi	The angle phi is extracted into this variable.
   //  xi      The angle xi is extracted into this variable.
	
 private:
};

inline Transform operator* (const Transform &firstTransfrom, const Transform &secondTransform);
 // Overloading binary operator * for multiplication between two transforms.

inline Transform inverse(const Transform &t);
 //  return  The inverse of the transform \a t.

inline Transform translation(double x, double y, double z);
 //  returns  A \a Transform representing a translation of \a x, \a y and \a z 
 //           units in the X, Y and Z directions.

Transform xRotation(double theta);
 //  returns  A \a Transform representing a rotation of angle 
 //          \a theta radians about the X axis.

Transform yRotation(double theta);
 //  returns  A \a Transform representing a rotation 
 //           of angle \a theta about the Y axis.

Transform zRotation(double theta);
 // returns a \a Transform representing 
 // a rotation of angle \a theta radians about the Z axis.

Transform vectorRotation(ColumnVector<3, double> &vector, double theta);
 // returns a \a Transform representing a rotation of 
 // angle \a theta radians about an arbitrary vector \a vector.

inline Transform eulerRotation(double theta, double phi, double xi);
 // Sets the rotational part of the transform matrix from the Euler 
 // angles \c phi, \c theta, \c xi.The rotational part
 // of the Transform matrix is obtained by first defining a rotation 
 // about Z axis by \c phi radians, then a rotation about the 
 // current Y axis by \c theta radians and finally a rotation about 
 // the current Z axis by \c xi radians.
 //  phi	Rotation about the Z axis.
 //  theta	Rotation about current Y axis (after rotation
 //		about \c phi).
 //  xi	Rotation about current Z axis (after rotation
 //		about \c phi and \c theta).

inline Transform rpyRotation(double roll, double pitch, double yaw);
 // Sets the rotational part of the transform matrix from the 
 // \c roll, \c pitch and \c yaw angles. The rotational part
 // of the Transform matrix is obtained by first defining 
 // a rotation about X axis by \c yaw radians, then a rotation 
 // about the Y axis by \c pitch radians and finally a rotation 
 // about the Z axis by \c roll radians, all rotations being 
 // relative to a fixed XYZ frame.
 //  yaw	Rotation about the X axis.
 //  pitch	Successive rotation about the Y axis.
 //  roll	Successive rotation about the Z axis.

// ========== END OF INTERFACE ==========


//============================================================= 
// Transform::Transform: Constructor of the Transform class
//=============================================================  
Transform::Transform()
{
 for (int row = 0; row < 4; row++)
  for (int column = 0; column < 4; column++)
   if(row == column)
    d_element[row][column] = 1;
   else
    d_element[row][column] = 0;
}


Transform::Transform(const Transform &transform)
{
 for (int row = 0; row < 4; row++)
  for (int column = 0; column < 4; column++)
   d_element[row][column] = transform.d_element[row][column];
}


//============================================================= 
// Transform::operator=
//=============================================================  
MatrixInitializer<double> Transform::operator=(const double &elementValue)
{
 d_element[0][0] = elementValue;
 MatrixInitializer<double>  matrixInitialize(16, 1, *d_element);
 return matrixInitialize;
}


//============================================================= 
// Transform::getTranslation
//=============================================================  
ColumnVector<3,double> Transform::getTranslation() const
{
 ColumnVector<3, double> translation;	 
 for (int position = 1; position <= 3; position++)
  translation(position)= d_element[position-1][3];
 return translation;
}


//============================================================= 
// Transform::getRollPitchYaw
//=============================================================  
void Transform::getRollPitchYaw(double &roll, double &pitch, double &yaw) const
{
 pitch = -asin(d_element[2][0]);
 yaw = asin((d_element[2][1])/cos(pitch));
 roll = asin((d_element[1][0])/cos(pitch));
}


//============================================================= 
// Transform::getEulerAngles
//=============================================================  
void Transform::getEulerAngles(double &theta, double &phi, double &xi) const
{
 theta = acos(d_element[2][2]);
 phi = asin((d_element[1][2])/sin(theta));
 xi = asin((d_element[2][1])/sin(theta));
}


//============================================================= 
// operator*
//=============================================================  
Transform operator* (const Transform &firstTransform, const Transform &secondTransform)
{
 Transform product;
 
 for (int row = 1; row <= 4; row++)
 {
  for (int column = 1; column <= 4; column++)
  {
   double productElement = 0;
   for (int commonDim = 1; commonDim <= 4; commonDim++)
    productElement += firstTransform(row, commonDim) * secondTransform(commonDim, column);
   product(row, column) = productElement;
  }
 }
 return product;
}


//============================================================= 
// inverse
//=============================================================  
Transform inverse(const Transform &transform)
{
 Transform invertedTransform;
 Matrix<3,3,double> rotation;
 ColumnVector<3, double> position;
	
 position = transform.getTranslation();
 transform.getSubMatrix(1,1,rotation);
 invertedTransform.setSubMatrix(1,1,(transpose(rotation)));
 
 invertedTransform(1,4) = -dotProduct(position, rotation.getColumn(1));
 invertedTransform(2,4) = -dotProduct(position, rotation.getColumn(2));
 invertedTransform(3,4) = -dotProduct(position, rotation.getColumn(3));

 return invertedTransform;
}


//============================================================= 
// translation
//=============================================================  
Transform translation(double x, double y, double z)
{
 Transform translation;
 for (int row = 1; row <= 4; row++)
 {
  for (int column = 1; column <= 4; column++)
  { 
   (row==column)?(translation(row,column)=1):(translation(row,column)=0);
  }
 }
 translation(1,4) = x;
 translation(2,4) = y;
 translation(3,4) = z;
 return translation;
}


//============================================================= 
// eulerRotation
//=============================================================  
Transform eulerRotation(double theta, double phi, double xi)
{
 Transform eulerRotation;
 eulerRotation = (zRotation(theta) * yRotation(phi)) * zRotation(xi);
 return eulerRotation;
}


//============================================================= 
// rpyRotation
//=============================================================  
Transform rpyRotation(double roll, double pitch, double yaw)
{
 Transform rpyRotation;
 rpyRotation = (zRotation(roll) * yRotation(pitch)) * xRotation(yaw);
 return rpyRotation;
}



#endif

