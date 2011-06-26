#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include <stdexcept>

using namespace subjugator;
using namespace Eigen;

Vector3d AttitudeHelpers::LocalGravity(double lat, double depth)
{
	// 6378137 is the approximate radius of the earth using a spherical model
	double g0 = (9.780318*(1+5.3024e-3*pow(sin(lat),2)-5.9e-6*pow(sin(2*lat),2))) /	pow((1 - depth / 6378137), 2);

	Vector3d g(0.0, 0.0, g0);

	return g;
}

double AttitudeHelpers::Markov_wStdDev(double dt, double T, double sigma)
{
	assert(T != 0);

	return (sigma*sqrt(2*dt/T - pow((dt/T),2)));
}

Vector3d AttitudeHelpers::RotationToEuler(const Matrix3d& R)
{
	if(R(2,0) == 1.0)
	{
		throw std::runtime_error("Euler angles are at singularity!");
	}

	Vector3d res(atan2(R(2,1),R(2,2)),
			-asin(R(2,0)),
			atan2(R(1,0),R(0,0)));

	return res;
}

double AttitudeHelpers::DAngleDiff(double a, double b)
{
	static double Pi = boost::math::constants::pi<double>();
	static double TwoPi = 2*Pi;

	double res = b-a;
	while(res < -1*Pi) res += TwoPi;
	while(res > Pi) res-= TwoPi;

	return res;
}

double AttitudeHelpers::DAngleClamp(double a)
{
	static double Pi = boost::math::constants::pi<double>();
	static double TwoPi = 2*Pi;

	double res = a;

	while(res < -1*Pi) res+=TwoPi;
	while(res > Pi) res-=TwoPi;

	return res;
}

Vector4d AttitudeHelpers::RotationToQuaternion(const Matrix3d& R)
{
	// Build the K matrix
	Matrix4d K;

	double K12 = R(1,0) + R(0,1);
	double K13 = R(2,0) + R(0,2);
	double K14 = R(1,2) - R(2,1);
	double K23 = R(2,1) + R(1,2);
	double K24 = R(2,0) - R(0,2);
	double K34 = R(0,1) - R(1,0);

	// First Row
	K(0,0) = R(0,0) - R(1,1) - R(2,2);
	K(0,1) = K12;
	K(0,2) = K13;
	K(0,3) = K14;

	// Second Row
	K(1,0) = K12;
	K(1,1) = R(1,1) - R(0,0) - R(2,2);
	K(1,2) = K23;
	K(1,3) = K24;

	// Third Row
	K(2,0) = K13;
	K(2,1) = K23;
	K(2,2) = R(2,2) - R(0,0) - R(1,1);
	K(2,3) = K34;

	// Fourth Row
	K(3,0) = K14;
	K(3,1) = K24;
	K(3,2) = K34;
	K(3,3) = R(0,0) + R(1,1) + R(2,2);

	K *= (1.0/3.0)*K;

	EigenSolver<Matrix4d> es(K);	// This computes both eigenvalues and eigenvectors. Sweet!
	Vector4d D = es.eigenvalues().real();		// get the real portion of the eigenvalues

	// The largest eigenvalue corresponds to the eigenvector we want
	int maxIndex = 0;
	double maxValue = 0;
	double temp;
	for(int i = 0; i < 4; i++)
	{
		if((temp = abs(D(i))) > maxValue)
		{
			maxValue = temp;
			maxIndex = i;
		}
	}

	// Reorder the eigenvector correctly - this yields the inverse rotation of what we're after
	Vector4d q_inv(es.eigenvectors().real()(3),
				   es.eigenvectors().real()(0),
				   es.eigenvectors().real()(1),
				   es.eigenvectors().real()(2));

	// return the inverse of the inverse rotation - Presto - the quaternion we wanted.
	return MILQuaternionOps::QuatInverse(q_inv);
}

Matrix3d VectorSkew3(const Vector3d& vec)
{
	Matrix3d res;

	res(0,1) = -vec(2);
	res(0,2) = vec(1);
	res(1,0) = vec(2);
	res(1,2) = -vec(0);
	res(2,0) = -vec(1);
	res(2,1) = vec(0);

	return res;
}

MatrixXd AttitudeHelpers::DiagMatrixFromVector(const VectorXd& v)
{
	MatrixXd res(v.rows(), v.rows());
	for(int i = 0; i < v.rows(); i++)
		res(i,i) = v(i);

	return res;
}
