#ifndef SUBNAVIGATIONCOMPUTER_H
#define SUBNAVIGATIONCOMPUTER_H

#include "LPOSVSS/SubThrusterCurrentCorrector.h"
#include "LPOSVSS/DataObjects/DVLVelocity.h"
#include "LPOSVSS/DataObjects/IMUInfo.h"
#include "LPOSVSS/DataObjects/DepthInfo.h"
#include "LPOSVSS/DataObjects/PDInfo.h"
#include "LPOSVSS/DataObjects/LPOSVSSInfo.h"
#include "LPOSVSS/Triad.h"
#include "LibSub/Math/AttitudeHelpers.h"
#include "LibSub/Math/Quaternion.h"
#include "LPOSVSS/SubINS.h"
#include "LPOSVSS/SubKalman.h"
#include <cmath>
#include <Eigen/Dense>

#include <time.h>

using namespace Eigen;

namespace subjugator
{
	class NavigationComputer
	{
	public:
		typedef Matrix<double,7,1> Vector7d;
	public:
		NavigationComputer();
		void Init(std::auto_ptr<IMUInfo> imuInfo, std::auto_ptr<DVLVelocity> dvlInfo, std::auto_ptr<DepthInfo> depthInfo, bool useDVL);
		bool getInitialized() { return initialized; }
		void UpdateIMU(const IMUInfo& imu);
		void UpdateDepth(const DepthInfo& dobj);
		void UpdateDVL(const DVLVelocity& dvl);
		void UpdateCurrents(const PDInfo& dobj);

		void Shutdown();
		void TarePosition(const Vector3d& position);
		void GetNavInfo(LPOSVSSInfo& info);

		void Update(boost::int64_t dtms);

	private:
		static const double latitudeDeg = 29.651388889; /*gainesville*/
		static const double alpha = 0.4082;
		static const double beta = 2.0;
		static const double kappa = 0;
		static const double bias_var_f = 0.000004;
		static const double bias_var_w = 26.2;
		static const double T_f = 5; // TODO
		static const double T_w = 100; // TODO
		static const double depth_sigma = 0.02;

		static const double MAX_DEPTH = 15; // m
		static const double MAX_DVL_NORM = 10; // Sub can't run at 10m/s

		Vector3d referenceNorthVector;
		Vector3d referenceGravityVector;
		Vector3d initialPosition;
		Vector3d initialVelocity;
		Vector3d white_noise_sigma_f;
		Vector3d white_noise_sigma_w;
		Vector3d dvl_sigma;
		Vector3d att_sigma;
		Vector4d q_SUB_DVL;
		Vector4d q_SUB_IMU;
		Vector4d q_MagCorrection;
		Vector3d magShift;
		Vector3d magScale;
		Matrix<double, 13, 13>covariance;

		std::vector<ThrusterCurrentCorrector> thrusterCurrentCorrectors;
		std::vector<double> thrusterCurrents;

		Vector4d q_MagCorrectionInverse;

		double depth_zero_offset;
		double depth_tare;

		bool depthRefAvailable;
		bool attRefAvailable;
		bool velRefAvailable;
		double depthRef;
		Vector4d attRef;
		Vector3d velRef;
		Vector7d z;
		Vector3d r_ORIGIN_NAV;

		Vector3d magSum;
		Vector3d accSum;
		double acceptable_gravity_mag;

		std::auto_ptr<Triad> triad;
		std::auto_ptr<KalmanFilter> kFilter;
		std::auto_ptr<INS> ins;

		bool initialized;

		bool shutdown;

		int kalmanCount;
		boost::int64_t kTimerMs;
		boost::int64_t kTimer;
		boost::int64_t dvlTimerMs;
		boost::int64_t dvlTimer;
		bool useDVL;
		void fakeDVL();

		void updateKalman();
		boost::int64_t getTimestamp(void);
		void resetErrors(bool tare, const Vector3d& tarePosition);
	};
}

#endif /* SUBNAVIGATIONCOMPUTER_H */
