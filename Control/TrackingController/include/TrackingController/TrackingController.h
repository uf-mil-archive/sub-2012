#ifndef SUBTRACKINGCONTROLLER_H
#define SUBTRACKINGCONTROLLER_H

#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "DataObjects/TrackingController/TrackingControllerInfo.h"
#include "DataObjects/Trajectory/TrajectoryInfo.h"
#include "LibSub/Math/EigenUtils.h"
#include <istream>

namespace subjugator {
	class TrackingController {
		public:
			struct Gains {
				Vector6d k;
				Vector6d ks;
				Vector6d alpha;
				Vector6d beta;
				Vector6d gamma1;
				Vector19d gamma2;
			};

			enum ControlTerms {
				TERM_PD = (1 << 0),
				TERM_RISE = (1 << 1),
				TERM_NN = (1 << 2)
			};

			enum Mode {
				MODE_PD = TERM_PD,
				MODE_RISE = TERM_RISE,
				MODE_RISE_NN = TERM_RISE | TERM_NN
			};

			struct Config {
				Gains gains;
				int mode;
			};

			TrackingController(const Config &config);
			void update(double dt, const TrajectoryInfo& traj, const LPOSVSSInfo& lposInfo, TrackingControllerInfo &info);

		private:
			const Config config;

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

	std::istream &operator>>(std::istream &in, TrackingController::Mode &mode);
}

#endif /* SUBTrackingController_H */
