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
}

#endif /* SUBKALMAN_H_ */
