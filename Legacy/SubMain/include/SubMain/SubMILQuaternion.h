#ifndef SUBMILQUATERNION_H
#define SUBMILQUATERNION_H

#include <Eigen/Dense>
#include <cmath>

using namespace Eigen;

// I ported this class over for a few reasons. The first of which is it follows the conventions
// for quaternion representation in Dr. Crane's book, and the methods that the INS, etc. were derived.
// Secondly, I have already debugged this code, and a major shift to eigens transformation based
// math looks too time consuming.
namespace subjugator
{
	class MILQuaternionOps
	{
	public:
		static Matrix3d Quat2Rot(const Vector4d& q);
		static Vector4d QuatNormalize(const Vector4d &q);
		static Vector4d QuatMultiply(const Vector4d& q, const Vector4d& p);
		static Vector4d QuatConjugate(const Vector4d& q);
		static Vector4d QuatInverse(const Vector4d& q);
		static Vector3d QuatRotate(const Vector4d& q, const Vector3d vec);
		static Vector3d Quat2Euler(const Vector4d& q);
	};
}

#endif /* SUBMILQUATERNION_H */
