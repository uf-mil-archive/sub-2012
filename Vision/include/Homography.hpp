//========================================================================
// Package	:
// Authors	: Vilas Kumar Chitrakaran
//  		  Optimal Homography compuation method originally
//                programmed by Naoya Ohta and Shimizu Yoshiyuki
//                (1999/2/25) of Computer Science Department,
//                Gunma University
// Start Date	: Wed Dec 20 11:08:28 GMT 2003
// Compiler	: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------
// File: Homography.hpp
//========================================================================
#ifndef INCLUDED_Homography_hpp
#define INCLUDED_Homography_hpp

#include "kinematic_math.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "Tensor3333.hpp"
#include <MathLibrary/VectorGSL.hpp>
#include <MathLibrary/RowVector.hpp>
#include "Math-Utility.hpp"
#include <gsl/gsl_linalg.h>
#include <math.h>


//========================================================================
// This file declares prototypes for functions to determine the 'projective'
// Homography matrix G from a set of pixel co-ordinates and to decompose
// 'euclidean' homography into translational and rotational components.
//========================================================================
extern VectorGSL<3> n_old1, n_old2;
// Numerical accuracy bounds
#define ERROR_BOUND	0.00001

void createHGWorkspace(int numPoints);
 // Creates workspace for calculations. Call before using rest of
 // the functions here.
 //  numPoints	Number of feature points as input to
 //             homography function.

void releaseHGWorkspace();
 // Deletes workspace. Call before exiting program.

//void tr2rpy(Matrix<3,3> &R , VectorGSL<3> &rpy) ;
//void tr2utheta(Matrix<3,3> &R , double &theta, VectorGSL<3> &u) ;

int getHomographySVD( MatrixBase<> &pi, MatrixBase<> &pi_star,
					Matrix<3, 3> &Gn, VectorBase<> &alpha_g33);
	// Determines normalized 'projective' homography 'Gn' between n
	// image coords pi and pi_star arranged as columns in the matrices of
	// dimension 3xn, using SVD/least squares method. Also outputs the
	// scale factor alpha_g33 for the n points of correspondance. This
	// function is an order faster than Homography determination using
	// Kanatani's method.
	//  returns  0 on success else -1.

int decomposeHomographySimple(Matrix<3,3> &H, VectorBase<> &alpha_g33,
					VectorGSL<3> &n_starActual, Matrix<3,3> &R_bar,
					VectorGSL<3> &x_h_bar, double &g33);

int decomposeHomographyKnownNstar(Matrix<3,3> &H,MatrixBase<> &mi_star,
						Matrix<3,3> &R_bar,
						VectorGSL<3> &x_f_bar,
						VectorGSL<3> &n_star,
						double &g33);

int decomposeHomography2Sol(Matrix<3,3> &H, MatrixBase<> &mi_star,
						Matrix<3,3> &R_bar,
						VectorGSL<3> &x_f_bar,
						VectorGSL<3> &n_star,
						double &g33);
	// Decomposes 'euclidean' homography H into its rotational component R_bar,
	// translation vector x_h_bar, distance g33 and the normal vector n_star
	// using Faugeras decomposition algorithm (p 290, 3D Computer Vision). Use
	// any mi_star to elimenate two solutions with points behind the camera,
	// return the other two solutions.  Returns 0 on success, else -1.
	// Note: The relationship between projective and euclidean homography is
	// G = A. H. A^T, where A is the camera calibration matrix.
	// where H = R_bar + (x_f_bar/g33) * (n_star)^T
	// THIS FUNCTION IS UN-TESTED YET.

int getHomographyVP(Matrix<3,8> &pi1, Matrix<3,8> &pi1_star,
				  Matrix<3,3> &Gn, VectorBase<> &alpha_g33);


#endif
