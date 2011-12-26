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
		WorkerMailbox<TrackingControllerGains> gainsmailbox;
		WorkerKillMonitor killmon;

		WorkerSignal<Vector6d> wrenchsignal;
		WorkerSignal<TrackingControllerInfo> infosignal;

	protected:
		void enterActive();
		void work(double dt);

	private:
		boost::scoped_ptr<TrackingController> controllerptr;

		void setControllerGains(const boost::optional<TrackingControllerGains> &gains);
	};
}

#endif
