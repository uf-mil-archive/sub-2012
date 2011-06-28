#ifndef SUBNAVIGATIONCOMPUTER_H
#define SUBNAVIGATIONCOMPUTER_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/LPOSVSS/SubThrusterCurrentCorrector.h"
#include "DataObjects/DVL/DVLHighresBottomTrack.h"
#include "DataObjects/IMU/IMUInfo.h"
#include "DataObjects/Depth/DepthInfo.h"
#include "SubMain/Workers/LPOSVSS/SubTriad.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubMILQuaternion.h"
#include "SubMain/Workers/LPOSVSS/SubINS.h"
#include "SubMain/Workers/LPOSVSS/SubKalman.h"
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
		NavigationComputer(boost::asio::io_service& io);
		void Init(std::auto_ptr<IMUInfo> imuInfo, std::auto_ptr<DVLHighresBottomTrack> dvlInfo, std::auto_ptr<DepthInfo> depthInfo, bool useDVL);
		bool getInitialized() { return initialized; }
		void Update(std::auto_ptr<IMUInfo> info);
		void Update(std::auto_ptr<DepthInfo> info);
		void Update(std::auto_ptr<DVLHighresBottomTrack> info);

		void TarePosition(Vector3d tarePosition);

	private:
		static const double latitudeDeg = 29.651388889; /*gainesville*/
		static const double alpha = 0.4082;
		static const double beta = 2.0;
		static const double kappa = 0;
		static const double bias_var_f = 0.01;
		static const double bias_var_w = 360;
		static const double T_f = 300;
		static const double T_w = 160;
		static const double depth_sigma = 0.002027;

		static const double MAX_DEPTH = 15; // m
		static const double MAX_DVL_NORM = 10; // Sub can't run at 10m/s

		boost::asio::io_service& io;

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

		bool useDVL;

		Vector4d q_MagCorrectionInverse;

		double depth_zero_tare;

		bool depthRefAvailable;
		bool attRefAvailable;
		bool velRefAvailable;
		double depthRef;
		Vector4d attRef;
		Vector3d velRef;
		Vector7d z;

		std::auto_ptr<Triad> triad;
		std::auto_ptr<KalmanFilter> kFilter;
		std::auto_ptr<INS> ins;

		bool initialized;

		boost::shared_mutex kLock;
		boost::shared_mutex tareLock;

		int kalmanCount;
		bool kalmanTimerInitialized;
		boost::int64_t kTimerMs;
		std::auto_ptr<boost::asio::deadline_timer> kTimer;
		void updateKalman(const boost::system::error_code& /*e*/);
		boost::uint64_t getTimestamp(void);
	};
}

#endif /* SUBNAVIGATIONCOMPUTER_H */
