#ifndef _TRACKINGCONTROLLERWORKER_H__
#define _TRACKINGCONTROLLERWORKER_H__

#include "DataObjects/TrackingController/ControllerGains.h"
#include "DataObjects/TrackingController/TrackingControllerInfo.h"
#include "HAL/HAL.h"
#include "LibSub/Math/EigenUtils.h"
#include "LibSub/State/State.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerConfigLoader.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include "LibSub/Worker/WorkerSignal.h"
#include "LibSub/Worker/WorkerKill.h"
#include "TrackingController/TrackingController.h"
#include <boost/scoped_ptr.hpp>

namespace subjugator
{
	class TrackingControllerWorker : public Worker {
	public:
		TrackingControllerWorker(const WorkerConfigLoader &configloader);

		WorkerMailbox<LPOSVSSInfo> lposvssmailbox;
		WorkerMailbox<TrajectoryInfo> trajectorymailbox;
		WorkerMailbox<TrackingController::Gains> gainsmailbox;
		WorkerKillMonitor killmon;

		WorkerSignal<Vector6d> wrenchsignal;
		WorkerSignal<TrackingControllerInfo> infosignal;

	protected:
		virtual void enterActive();
		virtual void work(double dt);

	private:
		const WorkerConfigLoader &configloader;

		boost::scoped_ptr<TrackingController> controllerptr;
		TrackingController::Config controllerconfig;

		void setControllerGains(const boost::optional<TrackingController::Gains> &gains);

		void loadConfig();
		void resetController();
		void setCurrentPosWaypoint();
	};
}

#endif
