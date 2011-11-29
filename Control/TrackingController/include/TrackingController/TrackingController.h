#ifndef SUBTRACKINGCONTROLLER_H
#define SUBTRACKINGCONTROLLER_H

#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "DataObjects/TrackingController/TrackingControllerInfo.h"
#include "DataObjects/Trajectory/TrajectoryInfo.h"
#include "LibSub/Math/EigenUtils.h"

namespace subjugator
{
	class TrackingControllerGains {
	public:
		Vector6d k;
		Vector6d ks;
		Vector6d alpha;
		Vector6d beta;
		Vector6d gamma1;
		Vector19d gamma2;
	};

	class TrackingController
	{
	public:
		TrackingController();

		void Update(double dt, const TrajectoryInfo& traj, const LPOSVSSInfo& lposInfo, TrackingControllerInfo &info);
		
		void SetGains(TrackingControllerGains new_gains);

	private:
		const bool rise_on;
		const bool nn_on;

		TrackingControllerGains gains;

		Vector6d rise_term_prev;
		Vector6d rise_term_int_prev;

		Vector6d xd_dot_prev;
		Vector6d xd_dotdot_prev;

		// The number of columns defines the number of hidden layer neurons in the controller, for now this is hardcoded at 5
		// Size = 19xN2
		Matrix19x5d V_hat_dot_prev;
		Matrix19x5d V_hat_prev;
		// Size = (N2+1)x6
		Matrix6d W_hat_dot_prev;
		Matrix6d W_hat_prev;

		Vector6d RiseFeedbackNoAccel(double dt, Vector6d e2);
		Vector6d NNFeedForward(double dt, Vector6d e2, Vector6d xd, Vector6d xd_dot);
		Vector6d PDFeedback(double dt, Vector6d e2);
	};
}

#endif /* SUBTrackingController_H */
