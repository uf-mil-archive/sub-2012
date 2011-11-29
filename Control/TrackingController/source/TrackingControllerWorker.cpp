#include <time.h>

#include "SubMain/SubAttitudeHelpers.h"
#include "SubMain/SubPrerequisites.h"

#include "TrackingController/TrackingControllerWorker.h"

using namespace std;
using namespace Eigen;
using namespace subjugator;

static boost::int64_t getTimestamp(void);

TrackingControllerWorker::TrackingControllerWorker(HAL &hal, const WorkerConfigLoader &configloader) :
	Worker("TrackingController", 50),
	lposvssmailbox("LPOSVSS", 1),
	hardwarekilledmailbox("PDStatus", numeric_limits<double>::infinity()),
	trajectorymailbox("Trajectory", numeric_limits<double>::infinity(), bind(&TrackingControllerWorker::setTrajectoryInfo, this, _1)),
	gainsmailbox("ControllerGains", numeric_limits<double>::infinity(), bind(&TrackingControllerWorker::setControllerGains, this, _1)),
	hardwarekilledchecker(&hardwarekilledmailbox)
{
	registerStateUpdater(lposvssmailbox);
	registerStateUpdater(hardwarekilledmailbox);
	registerStateUpdater(hardwarekilledchecker);
}

void TrackingControllerWorker::enterActive() {
	lock.lock();

	trackingController = std::auto_ptr<TrackingController>(new TrackingController());

	Vector6d traj;
	traj.head<3>() = lposvssmailbox.getOptional().get().getPosition_NED();
	traj(3) = traj(4) = 0;
	traj(5) = MILQuaternionOps::Quat2Euler(lposvssmailbox.getOptional().get().getQuat_NED_B())(2);
	trajInfo = auto_ptr<TrajectoryInfo>(new TrajectoryInfo(getTimestamp(), traj, Vector6d::Zero()));

	lock.unlock();
}

void TrackingControllerWorker::work(double dt) {
	// Protect against debugger and non-monotonic time
	if(dt <= 0 || dt > .150)
		return;

	lock.lock();

	TrackingControllerInfo info(getState().code, getTimestamp());
	trackingController->Update(dt, *trajInfo, lposvssmailbox.getOptional().get(), info);

	lock.unlock();

	// Emit every iteration
	wrenchsignal.emit(info.Wrench);
	infosignal.emit(info);
}

static boost::int64_t getTimestamp(void) {
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * 1e9) + t.tv_nsec;
}

void TrackingControllerWorker::setTrajectoryInfo(const boost::optional<TrajectoryInfo> &info) {
	lock.lock();

	if(info)
		trajInfo = std::auto_ptr<TrajectoryInfo>(new TrajectoryInfo(info.get()));

	lock.unlock();
}

void TrackingControllerWorker::setControllerGains(const boost::optional<TrackingControllerGains> &new_gains) {
	lock.lock();

	if(new_gains)
		if(trackingController.get())
			trackingController->SetGains(new_gains.get());

	lock.unlock();
}
