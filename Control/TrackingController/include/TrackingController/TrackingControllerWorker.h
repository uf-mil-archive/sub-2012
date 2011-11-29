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

#include "TrackingController/TrackingController.h"

namespace subjugator
{
	class HardwareKilledChecker : public StateUpdater {
	public:
		HardwareKilledChecker(WorkerMailbox<bool> *mailbox) : mailbox(mailbox) {};

		virtual const State &getState() const {
			return state;
		}

		virtual void updateState(double dt) {
			state = mailbox->get(true) ? State(State::STANDBY, "Waiting to be unkilled") : State(State::ACTIVE);
		}

	private:
		WorkerMailbox<bool> *mailbox;
		State state;
	};

	class TrackingControllerWorker : public Worker {
	public:
		TrackingControllerWorker(HAL &hal, const WorkerConfigLoader &configloader);

		WorkerMailbox<LPOSVSSInfo> lposvssmailbox;
		WorkerMailbox<bool> hardwarekilledmailbox;
		WorkerMailbox<TrajectoryInfo> trajectorymailbox;
		WorkerMailbox<TrackingControllerGains> gainsmailbox;

		WorkerSignal<Vector6d> wrenchsignal;
		WorkerSignal<TrackingControllerInfo> infosignal;

	protected:
		void enterActive();
		void work(double dt);

	private:
		std::auto_ptr<TrackingController> trackingController;

		std::auto_ptr<TrajectoryInfo> trajInfo;

		boost::mutex lock;

		HardwareKilledChecker hardwarekilledchecker;

		void setTrajectoryInfo(const boost::optional<TrajectoryInfo> &info);
		void setControllerGains(const boost::optional<TrackingControllerGains> &gains);
	};
}

#endif
