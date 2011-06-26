#ifndef SUBKALMAN_H
#define SUBKALMAN_H

#include "SubMain/SubPrerequisites.h"
#include <Eigen/Dense>

using namespace Eigen;

namespace subjugator
{
	class KalmanData
	{
	public:
		double DepthError;
		Vector3d VelocityError;
		Vector4d QuaternionError;
		Vector3d Acceleration_bias;
		Vector3d Gyro_bias;
		Vector3d PositionErrorEst;	// This also has depth error superimposed!

		KalmanData();
		KalmanData(double depthError, Vector3d velError, Vector4d quatError, Vector3d a_bias, Vector3d w_bias, Vector3d pestErr) :
			DepthError(depthError), VelocityError(velError), QuaternionError(quatError), Acceleration_bias(a_bias),
			Gyro_bias(w_bias), PositionErrorEst(pestErr)
		{
		}
	};

	class KalmanFilter
	{
	public:
		typedef Matrix<double, 13, 1> Vector13d;	// x_hat
		typedef Matrix<double, 1, 26> RowVector26d;	// helper vectors
		typedef Matrix<double, 1, 27> RowVector27d;
		typedef Matrix<double, 13, 13> Matrix13d;
		typedef Matrix<double, 7, 7> Matrix7d;
		//typedef Matrix<double, >
	public:


		//public Vector3d getP_Error() { return }

	private:
		boost::mutex lock;
		boost::uint64_t prevTickCount;

		RowVector27d ones2LXp1;
		RowVector26d ones2LX;
		double gravityMag;

		int L;
		Vector13d x_hat;

		// Internally the units of the noise for the gyro and IMU are converted to match the units of the Kalman filter
		// provided they are specified as documented in the matlab sensorCharacteristics file.
		Vector4d q_hat;


		Matrix13d P_Hat;
	};
}

#endif /* SUBKALMAN_H_ */
