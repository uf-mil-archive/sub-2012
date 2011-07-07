#ifndef POSE_EST_H
#define POSE_EST_H

#include "kinematic_math.h"
#include "Homography.hpp"

//THIS IS THE POSE ESTIMATION FUNCTION
//It works by assigning an orientation Rstar to the reference plane and estimates dstar as well
//it also returns an estimate of the 3D coordinates of the points in th camera frame
//R_ref is the output orientation, mi_ref_bar will contain the translation, it corresponds to whichever
//point is considered the origin of the target.
//
//see N Gans technical note for details
void find_Rstar_dstar( int ind1, int ind2, double s_bar, Matrix<3,4> mi_ref, VectorGSL<3> n_ref, Matrix<3,3> &R_ref, Matrix<3,4> &mi_ref_bar, double &d_ref);
void find_Rstar_dstar( int ind1, int ind2, double s_bar, Matrix<3,7> mi_ref, VectorGSL<3> n_ref, Matrix<3,3> &R_ref, Matrix<3,7> &mi_ref_bar, double &d_ref);


//It takes two views of features points in pixel coordintes, the features in the in the current view are mi, and the features in a reference view
// are mi_ref.  It also takes the known 3D length between the first and second point.  It returns the Rotation matrix Rot and
//translation vector Trans from the camera frame to a frame attached to the first point in mi, and oriented such that the z-axis points
//along the normal of the feature point plane and the x-axis is along the line from the elem_ind to the elem_ind+1 points of mi.
//elem_int is input to determine which line between two points is used to determine orientation
//cur_frame is a flag.  0 means it returns the pose of the planar target in the reference image, 1 means it returns the pose in the current image.
//There is an overridden version that returns the solved componants of the Homography
void estimate_Pose(Matrix<3,3> A,Matrix<3,3> invA, Matrix<3,4> pi_ref, Matrix<3,4> pi,  double ds1_s2[4], Matrix<3,3> &Rot, VectorGSL<3> &Trans,int elem_ind,int current_frame);
void estimate_Pose(Matrix<3,3> A,Matrix<3,3> invA, Matrix<3,4> pi_ref, Matrix<3,4> pi,  double ds1_s2[4], Matrix<3,3> &Rot,
				   VectorGSL<3> &Trans, VectorGSL<4> &alpha, Matrix<3,3> &R_bar, VectorGSL<3> &x_bar, VectorGSL<3> &n_refActual, int elem_ind, int current_frame );
#endif
