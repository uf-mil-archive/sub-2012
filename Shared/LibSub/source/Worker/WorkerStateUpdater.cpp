#include "LibSub/Worker/WorkerStateUpdater.h"
#include <boost/bind.hpp>
#include <algorithm>

using namespace subjugator;

const WorkerState &WorkerStateUpdaterContainer::getWorkerState() const { return state; }

void WorkerStateUpdaterContainer::updateState(double dt) {
	state = WorkerState();
	for (UpdaterVec::const_iterator i = updatervec.begin(); i != updatervec.end(); ++i) {
		WorkerStateUpdater &updater = **i;
		updater.updateState(dt);
		state = state.combine(updater.getWorkerState());
	}
}

