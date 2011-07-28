#ifndef SUBATTITUDEHELPERS_H_
#define SUBATTITUDEHELPERS_H_

#include <Eigen/Dense>
#include <cmath>
#include <boost/math/constants/constants.hpp>
#include "SubMain/Workers/LPOSVSS/SubMILQuaternion.h"

using namespace Eigen;

namespace subjugator
{
	class AttitudeHelpers
	{
	public:
		static Vector3d LocalGravity(double lat, double depth);
		static double Markov_wStdDev(double dt, double T, double sigma);
		static Vector3d RotationToEuler(const Matrix3d& R);
		static double DAngleDiff(double a, double b);
		static double DAngleClamp(double a);
		static Vector4d RotationToQuaternion(const Matrix3d& R);
		static Matrix3d VectorSkew3(const Vector3d& v);
		static MatrixXd DiagMatrixFromVector(const VectorXd& v);
		static VectorXd Tanh(const VectorXd& v);
		static VectorXd Sech(const VectorXd& v);
	};
}

#endif /* SUBATTITUDEHELPERS_H_ */
