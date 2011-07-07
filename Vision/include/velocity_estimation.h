////////////////////////////////////////////////
//	Velocity ID Header File
////////////////////////////////////////////////

#ifndef INCLUDE_velocity_functions_h
#define INCLUDE_velocity_functions_h

#include <MathLibrary/Matrix.hpp>
#include <MathLibrary/VectorGSL.hpp>

//=================================================================
//Construct the inverse of the Jacobian-like 6X6 matrix 
//also contrustc the error vector e
//=================================================================
Matrix <6,6> constructInvJ(double alpha, Matrix<3,3> A, Matrix<3,3> R_bar, 
						VectorGSL<3> p1, VectorGSL<3> p1_star, VectorGSL<3> xf_star, 
						Matrix<3,3> R_star, VectorGSL<3> s1,VectorGSL<6> &e);

//=================================================================
//Construct Jacobian-like 6X6 matrix 
//This function is somewhat obsolete, since what we really need is 
//J^-1, and we can calculate that in closed form without numerical 
//integration.
//also contrustc the error vector e
//=================================================================
Matrix <6,6> constructJ( double alpha, Matrix<3,3> A, Matrix<3,3> R_bar, 
						VectorGSL<3> p1, VectorGSL<3> p1_star, VectorGSL<3> xf_star, 
						Matrix<3,3> R_star, VectorGSL<3> s1,VectorGSL<6> &e);

//k_hat is the estimated term.  You can try trapezoidal or Simpson's rule for integrating.  Trapezoidal should be better for piecewise linear inputs,
//Simpson is better for polynomial inputs
//====================================================================
// Construct k_hat(t) using Simpson's rule integration
//====================================================================
VectorGSL<6> getk_hat_Simpson( VectorGSL<6> K, VectorGSL<6> rho, double sampling_period, VectorGSL<6> &e,  VectorGSL<6> &y1_current, VectorGSL<6> &y2_current,
				   VectorGSL<6> &e_tilde_previous1, VectorGSL<6> &e_tilde_sig_previous1,VectorGSL<6> &e_tilde_previous2, VectorGSL<6> &e_tilde_sig_previous2,
					 VectorGSL<6> &e_hat_current, VectorGSL<6> &e_hat_dot_prev1,VectorGSL<6> &e_hat_dot_prev2,  VectorGSL<6> &e_hat_dot_current );

//====================================================================
// Construct k_hat(t) - trapezoidal rule integration
//====================================================================
VectorGSL<6> getk_hat( VectorGSL<6> K, VectorGSL<6> rho, double sampling_period, VectorGSL<6> &e,  VectorGSL<6> &y1_current, VectorGSL<6> &y2_current,
				   VectorGSL<6> &e_tilde_previous, VectorGSL<6> &e_tilde_sig_previous,
					 VectorGSL<6> &e_hat_current, VectorGSL<6> &e_hat_dot_prev,  VectorGSL<6> &e_hat_dot_current);

//======================================================================
// Simpsons rule for integration of a matrix or vector
//======================================================================
void Integrate_Matrix_Simpson(double dt, VectorGSL<6> &vect_int_current, VectorGSL<6> &vect_func_prev1, 
							  VectorGSL<6> &vect_func_prev2, VectorGSL<6> &vect_func_current);

//======================================================================
// Trapazoid rule for integration of a matrix or vector
//======================================================================
void Integrate_Matrix(double dt, VectorGSL<6> &vect_int_current, VectorGSL<6> &vect_func_prev,  VectorGSL<6> &vect_func_current);

//======================================================================
// Takes a 6 element vector, changes it into a diagonal 6x6 matrix and returns it.
//======================================================================
Matrix<6,6> diagMatrix(VectorGSL<6> a);

//=================================================================
// Returns the Signum value for k_hat calculation
// Returns -1,0, or 1 for funtion e(t)
//=================================================================
VectorGSL<6> Signum ( VectorGSL<6> inputmatrix );
	

#endif
