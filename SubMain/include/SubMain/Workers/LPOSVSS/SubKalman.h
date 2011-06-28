#ifndef SUBKALMAN_H
#define SUBKALMAN_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubMILQuaternion.h"
#include <Eigen/Dense>
#include <cmath>

using namespace Eigen;

namespace subjugator
{
	class KalmanData
	{
	public:
		double DepthError;
		Vector3d VelocityError;
		Vector4d ErrorQuaternion;
		Vector3d Acceleration_bias;
		Vector3d Gyro_bias;
		Vector3d PositionErrorEst;	// This also has depth error superimposed!

		KalmanData();
		KalmanData(double depthError, Vector3d velError, Vector4d errorQuat, Vector3d a_bias, Vector3d w_bias, Vector3d pestErr) :
			DepthError(depthError), VelocityError(velError), ErrorQuaternion(errorQuat), Acceleration_bias(a_bias),
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
		typedef Matrix<double, 7, 1> Vector7d;
		typedef Matrix<double, 13, 7> Matrix13x7d;
		typedef Matrix<double, 13, 12> Matrix13x12d;
		typedef Matrix<double, 13, 26> Matrix13x26d;
		typedef Matrix<double, 13, 27> Matrix13x27d;
		typedef Matrix<double, 12, 13> Matrix12x13d;
		typedef Matrix<double, 12, 12> Matrix12d;
		typedef Matrix<double, 4, 27> Matrix4x27d;
		typedef Matrix<double, 3,27> Matrix3x27d;
		typedef Matrix<double, 7, 27> Matrix7x27d;
		typedef Matrix<double, 7, 26> Matrix7x26d;
	public:
		KalmanFilter(int L, double gravityMag, Vector4d q_hat, Matrix13d P_hat,
					 double alpha, double beta, double kappa, double bias_var_f, double bias_var_w,
					 Vector3d white_noise_sigma_f, Vector3d white_noise_sigma_w, double T_f,
					 double T_w, double depth_sigma, Vector3d dvl_sigma, Vector3d att_sigma,
					 boost::uint64_t startTickCount);
		void Update(const Vector7d& z, const Vector3d& f_IMU,
					 const Vector3d& v_INS, const Vector4d& q_INS, boost::uint64_t currentTickCount);
		void Reset();
		boost::shared_ptr<KalmanData> GetData()
		{
			lock.lock();
			boost::shared_ptr<KalmanData> temp(prevData);
			lock.unlock();

			return temp;
		}
	private:
		static const double SECPERNANOSEC = 1e-9;

		boost::mutex lock;
		bool initialized;

		RowVector27d ones2LXp1;
		RowVector26d ones2LX;

		int L;
		double gravityMag;

		Vector13d x_hat;

		// Internally the units of the noise for the gyro and IMU are converted to match the units of the Kalman filter
		// provided they are specified as documented in the matlab sensorCharacteristics file.
		Vector4d q_hat;

		Matrix13d P_hat;
		Matrix7d R;
		Matrix13x7d K;
		Vector3d P_est_error;

		Matrix13x12d gamma;
		Matrix12x13d gammaTransposed;

		double alpha;
		double beta;
		double kappa;
		double lambda;

		double W_s[2];
		double W_c[2];

		Matrix3d white_bias_sigma_f;
		double bias_var_f;
		Matrix3d T_f_inv;

		Matrix3d white_bias_sigma_w;
		double bias_var_w;
		Matrix3d T_w_inv;

		double T_f;
		double T_w;
		boost::uint64_t prevTickCount;

		boost::shared_ptr<KalmanData> prevData;
	};
}

#endif /* SUBKALMAN_H_ */
