#ifndef SUBINS_H
#define SUBINS_H

#include "SubMain/SubPrerequisites.h"
#include "DataObjects/IMU/IMUInfo.h"
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <math.h>
#include "SubMain/Workers/LPOSVSS/SubMILQuaternion.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubKalman.h"

using namespace Eigen;

namespace subjugator
{
	struct INSData
	{
	public:
		INSData();
		INSData(Vector3d p, Vector3d v, Vector4d q, Vector3d g_body, Vector3d a_body, Vector3d w_body,
				Vector3d a_bias, Vector3d w_bias)
		: Position_NED(p), Velocity_NED(v), Quaternion(q), Gravity_BODY(g_body),
					Acceleration_BODY(a_body), AngularRate_BODY(w_body), AccelerationBias(a_bias),
					AngularRateBias(w_bias){}

		Vector3d Position_NED;
		Vector3d Velocity_NED;
		Vector4d Quaternion;
		Vector3d Gravity_BODY;
		Vector3d Acceleration_BODY;
		Vector3d AngularRate_BODY;
		Vector3d AccelerationBias;
		Vector3d AngularRateBias;
	};

	class INS
	{
	public:
		INS(double lat, Vector3d w_dif_prev, Vector3d a_body_prev, Vector3d p_prev,
			Vector3d v_prev, Vector3d g, Vector4d q_prev, Vector3d w_bias, Vector3d a_bias,
			Vector4d q_SUB_IMU, double imuTime);

		boost::shared_ptr<INSData> Update(const std::auto_ptr<IMUInfo> info);
		boost::shared_ptr<INSData> Reset(KalmanData& kData, bool tare, Vector3d tarePosition);

	private:
		static const double SECPERNANOSEC = 1e-9;
		static const double r_earth = 6378137;    // Radius of the earth (m)
		static const double w_ie_e = 7.292115e-5;  // Angular rate of the earth (rad/s

		boost::mutex lock;
		bool initialized;

		double lat;
		Vector3d w_dif_prev;
		Vector3d a_body_prev;
		Vector3d p_prev;
		Vector3d v_prev;
		Vector3d g;
		double gMag;
		Vector4d q_prev;

		Vector3d w_bias;
		Vector3d a_bias;
		Vector4d q_SUB_IMU;
		boost::shared_ptr<INSData> prevData;

		double dt;
		boost::uint64_t imuPreviousTime;
		Vector3d w_ie_n;
		Vector3d w_en_n;

		Vector3d p;
		Vector3d v;
		Vector4d q;
	};
}

#endif /* SUBINS_H */
