#ifndef TRACKINGCONTROLLER_TRACKINGCONTROLLERWORKER_H
#define TRACKINGCONTROLLER_TRACKINGCONTROLLERWORKER_H

#include "TrackingController/TrackingController.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerConfigLoader.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include "LibSub/Worker/WorkerSignal.h"
#include "LibSub/Worker/WorkerKill.h"
#include "LibSub/Math/EigenUtils.h"
#include "LibSub/State/State.h"
#include <boost/scoped_ptr.hpp>

namespace subjugator {
	class TrackingControllerWorker : public Worker {
		public:
			struct LPOSVSSInfo {
				Vector3d position_ned;
				Vector4d quaternion_ned_b;
				Vector3d velocity_ned;
				Vector3d angularrate_body;
			};

			struct LogData {
				Matrix19x5d v_hat;
				Matrix6d w_hat;

				TrackingController::Output out;
			};

			TrackingControllerWorker(const WorkerConfigLoader &configloader);

			WorkerMailbox<LPOSVSSInfo> lposvssmailbox;
			WorkerMailbox<TrackingController::TrajectoryPoint> trajectorymailbox;
			WorkerMailbox<TrackingController::Gains> gainsmailbox;
			WorkerKillMonitor killmon;

			WorkerSignal<Vector6d> wrenchsignal;
			WorkerSignal<LogData> logsignal;
			WorkerSignal<TrackingController::Gains> gainssignal;
			WorkerSignal<TrackingController::TrajectoryPoint> initialpointsignal;

		protected:
			virtual void enterActive();
			virtual void initialize();
			virtual void work(double dt);

		private:
			boost::scoped_ptr<TrackingController> controllerptr;
			TrackingController::Config controllerconfig;

			void setControllerGains(const boost::optional<TrackingController::Gains> &gains);

			void loadConfig();
			void saveConfigGains() const;
			void resetController();
			void setCurrentPosWaypoint();
	};
}

#endif

