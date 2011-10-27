#ifndef SUBTRACKINGCONTROLLER_H
#define SUBTRACKINGCONTROLLER_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubMILQuaternion.h"
#include "DataObjects/Trajectory/TrajectoryInfo.h"
#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "DataObjects/TrackingController/TrackingControllerInfo.h"
#include <Eigen/Dense>
#include <cmath>

using namespace Eigen;

namespace subjugator
{
	class TrackingController
	{
	public:
		typedef Matrix<double, 6, 1> Vector6d;
		typedef Matrix<double, 6, 6> Matrix6d;
		typedef Matrix<double, 19, 1> Vector19d;
		typedef Matrix<double, 19, 19> Matrix19d;
		typedef Matrix<double, 19, 5> Matrix19x5d;
		typedef Matrix<double, Dynamic, Dynamic> MatrixXd;
		typedef Matrix<double, Dynamic, Dynamic> VectorXd;
	public:
		TrackingController();

		void GetWrench(TrackingControllerInfo &info);
		void Update(boost::int64_t currentTick, const TrajectoryInfo& traj, const LPOSVSSInfo& lposInfo);
		void InitTimer(boost::int64_t currentTickCount);
		
		void SetGainsTemp(Vector6d kV, Vector6d ksV, Vector6d alphaV, Vector6d betaV);
		
		void SetGains(Vector6d kV, Vector6d ksV, Vector6d alphaV, Vector6d betaV, const LPOSVSSInfo& lposInfo);

	private:
		static const double SECPERNANOSEC = 1e-9;

		Matrix6d k;
		Matrix6d ks;
		Matrix6d ksPlus1;
		Matrix6d alpha;
		Matrix6d beta;
		Matrix6d gamma1; // Size = (N2+1)x(N2+1)
		Matrix19d gamma2; // Size = 19x19

		Vector6d ktemp;
		Vector6d kstemp;
		Vector6d alphatemp;
		Vector6d betatemp;
		Vector6d gamma1temp;
		Vector19d gamma2temp;
		Vector6d pd_control;

		Vector6d e;
		Vector6d e2;
		Vector6d r;
		Vector6d rise_term;
		Vector6d rise_term_int;
		Vector6d rise_term_prev;
		Vector6d rise_term_int_prev;
		Vector6d rise_control;

		Vector6d xd;
		Vector6d xd_dot;
		Vector6d xd_dot_prev;
		Vector6d xd_dotdot;
		Vector6d xd_dotdot_prev;
		Vector6d xd_dotdotdot;

		// The number of columns defines the number of hidden layer neurons in the controller, for now this is hardcoded at 5
		// Size = 19xN2
		Matrix19x5d V_hat_dot;
		Matrix19x5d V_hat_dot_prev;
		Matrix19x5d V_hat;
		Matrix19x5d V_hat_prev;
		// Size = (N2+1)x6
		Matrix6d W_hat_dot;
        Matrix6d W_hat_dot_prev;
        Matrix6d W_hat;
        Matrix6d W_hat_prev;
        Vector6d nn_control;

		Matrix6d J;
		Matrix6d J_inv;

		Vector6d x;
		Vector6d x_dot;

		Vector6d vb;

		bool pd_on;
		bool rise_on;
		bool nn_on;

		boost::int64_t previousTime;

		Vector6d currentControl;
		boost::mutex lock;

		void UpdateJacobian(const Vector6d& x);
		void UpdateJacobianInverse(const Vector6d& x);
		Vector6d RiseFeedbackNoAccel(double dt);
		Vector6d NNFeedForward(double dt);
		Vector6d PDFeedback(double dt);
		Vector6d GetSigns(const Vector6d& x);
	};
}



#endif /* SUBTrackingController_H */
