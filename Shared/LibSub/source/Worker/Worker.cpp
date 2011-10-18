#include "LibSub/Worker/Worker.h"

using namespace subjugator;
using namespace boost;
using namespace std;

void Worker::update(double dt) {
	WorkerState oldstate = getWorkerState();
	updateState(dt);
	const WorkerState &newstate = getWorkerState();

	if (oldstate != newstate) {
		if (newstate.code == WorkerState::ACTIVE)
			enterActive();
		else if (oldstate.code == WorkerState::ACTIVE)
			leaveActive();

		statechangedsig.emit(make_pair(oldstate, newstate));
	}

	if (newstate.code == WorkerState::ACTIVE)
		work(dt);
}

void Worker::work(double dt) { }

void Worker::enterActive() { }

void Worker::leaveActive() { }

