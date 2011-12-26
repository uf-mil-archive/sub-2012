#include <time.h>

#include "SubMain/SubAttitudeHelpers.h"
#include "SubMain/SubPrerequisites.h"

#include "TrackingController/TrackingControllerWorker.h"

using namespace std;
using namespace Eigen;
using namespace subjugator;

static boost::int64_t getTimestamp(void);

TrackingControllerWorker::TrackingControllerWorker(const WorkerConfigLoader &configloader) :
	Worker("TrackingController", 50),
	lposvssmailbox(WorkerMailbox<LPOSVSSInfo>::Args()
		.setName("LPOSVSS")
		.setMaxAge(.2)),
	trajectorymailbox(WorkerMailbox<TrajectoryInfo>::Args()
		.setName("Trajectory")),
	gainsmailbox(WorkerMailbox<TrackingControllerGains>::Args()
		.setName("ControllerGains")
		.setCallback(bind(&TrackingControllerWorker::setControllerGains, this, _1)))
{
	registerStateUpdater(lposvssmailbox);
	registerStateUpdater(killmon);
}

void TrackingControllerWorker::enterActive() {
	controllerptr.reset(new TrackingController());

	const LPOSVSSInfo &lpos = *lposvssmailbox.getOptional();
	Vector6d traj;
	traj.head<3>() = lpos.getPosition_NED();
	traj(3) = traj(4) = 0;
	traj(5) = MILQuaternionOps::Quat2Euler(lpos.getQuat_NED_B())(2);

	trajectorymailbox.set(TrajectoryInfo(getTimestamp(), traj, Vector6d::Zero()));
}

void TrackingControllerWorker::work(double dt) {
	// Protect against debugger and non-monotonic time
	if (dt <= 0 || dt > .150)
		return;

	TrackingControllerInfo info(getState().code, getTimestamp());
	controllerptr->Update(dt, *trajectorymailbox.getOptional(), *lposvssmailbox.getOptional(), info);

	// Emit every iteration
	wrenchsignal.emit(info.Wrench);
	infosignal.emit(info);
}

static boost::int64_t getTimestamp(void) {
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * 1e9) + t.tv_nsec;
}

void TrackingControllerWorker::setControllerGains(const boost::optional<TrackingControllerGains> &new_gains) {
	if (new_gains && controllerptr)
		controllerptr->SetGains(*new_gains);
}
