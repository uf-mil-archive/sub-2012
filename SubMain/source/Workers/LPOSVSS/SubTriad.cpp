#include "SubMain/Workers/LPOSVSS/SubTriad.h"

using namespace subjugator;
using namespace Eigen;

Triad::Triad(const Vector4d& q, const Vector3d& v1_NED, const Vector3d& v2_NED)
	:Quaternion(q)
{
	R_NED_T.col(0) = v1_NED;
	R_NED_T.col(0).normalize();
	R_NED_T.col(1) = v1_NED.cross(v2_NED);
	R_NED_T.col(1).normalize();
	R_NED_T.col(2) = v1_NED.cross(R_NED_T.col(1));
	R_NED_T.col(2).normalize();
}

Vector4d Triad::Update(const Vector3d& v1_BOD, const Vector3d& v2_BOD)
{
	// Build the T_BOD rotation matrix
	Matrix3d R_BOD_T;
	R_BOD_T.col(0) = v1_BOD;
	R_BOD_T.col(0).normalize();
	R_BOD_T.col(1) = v1_BOD.cross(v2_BOD);
	R_BOD_T.col(1).normalize();
	R_BOD_T.col(2) = v1_BOD.cross(R_BOD_T.col(1));
	R_BOD_T.col(2).normalize();

	// R_BOD_T^-1 = R_T_BOD, but since they are orthogonal
	// R_BOD_T^-1 = R_BOD_T'
	Matrix3d R_NED_BOD = R_NED_T * R_BOD_T.transpose();

	// Now, we need to get the quaternion that represents the rotation matrix
	Vector4d q = AttitudeHelpers::RotationToQuaternion(R_NED_BOD);

	// Attitude helpers ensures the scalar portion is positive, and normalized
	return q;
}
