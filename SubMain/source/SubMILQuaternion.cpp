#include "SubMain/SubMILQuaternion.h"
#include "SubMain/SubAttitudeHelpers.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

Matrix3d MILQuaternionOps::Quat2Rot(const Vector4d& q)
{
	Matrix3d rot;

	double q0_1 = q(0)*q(1);
	double q0_2 = q(0)*q(2);
	double q0_3 = q(0)*q(3);
	double q1_2 = q(1)*q(2);
	double q1_3 = q(1)*q(3);
	double q2_3 = q(2)*q(3);
	double q1_1 = q(1)*q(1);
	double q2_2 = q(2)*q(2);
	double q3_3 = q(3)*q(3);

	rot(0,0) = 1-2*(q2_2+q3_3);
	rot(0,1) = 2*(q1_2-q0_3);
	rot(0,2) = 2*(q1_3+q0_2);
	rot(1,0) = 2*(q1_2+q0_3);
	rot(1,1) = 1-2*(q1_1+q3_3);
	rot(1,2) = 2*(q2_3-q0_1);
	rot(2,0) = 2*(q1_3-q0_2);
	rot(2,1) = 2*(q2_3+q0_1);
	rot(2,2) = 1-2*(q1_1+q2_2);

	return rot;
}

// Based on eigen recommendations
Vector4d MILQuaternionOps::QuatNormalize(const Vector4d &q)
{
	double mag = sqrt(q(0)*q(0)+q(1)*q(1)+q(2)*q(2)+q(3)*q(3));

	return (q *(1.0 / mag));
}

Vector4d MILQuaternionOps::QuatMultiply(const Vector4d& q, const Vector4d& p)
{
	Vector4d res;

	res(0) = p(0)*q(0)-q(1)*p(1)-q(2)*p(2)-q(3)*p(3);
	res(1) = q(0)*p(1)+q(1)*p(0)+q(2)*p(3)-q(3)*p(2);
	res(2) = q(0)*p(2)+q(2)*p(0)-q(1)*p(3)+q(3)*p(1);
	res(3) = q(0)*p(3)+q(3)*p(0)+q(1)*p(2)-q(2)*p(1);

	return res;
}

Vector4d MILQuaternionOps::QuatConjugate(const Vector4d& q)
{
	Vector4d res;

	res(0) = q(0);
	res(1) = -q(1);
	res(2) = -q(2);
	res(3) = -q(3);

	return res;
}

Vector4d MILQuaternionOps::QuatInverse(const Vector4d& q)
{
	return QuatNormalize(QuatConjugate(q));
}

Vector3d MILQuaternionOps::QuatRotate(const Vector4d& q, const Vector3d vec)
{
	// Make sure the quaternion is normalized
	Vector4d qNorm = QuatNormalize(q);

	// Expand the vector into 4x1 quaternion
	Vector4d r(0.0, vec(0), vec(1), vec(2));

	Vector4d intermediate = QuatMultiply(q,r);
	intermediate = QuatMultiply(intermediate, QuatConjugate(q));

	Vector3d res(intermediate(1),intermediate(2), intermediate(3));

	return res;
}

Vector3d MILQuaternionOps::Quat2Euler(const Vector4d& q)
{
	return AttitudeHelpers::RotationToEuler(Quat2Rot(q));
}
