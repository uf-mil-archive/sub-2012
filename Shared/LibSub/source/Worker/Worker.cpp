#include "LibSub/Worker/Worker.h"

using namespace subjugator;
using namespace boost;
using namespace std;

void Worker::update(double dt) {
	State oldstate = getState();
	updateState(dt);
	const State &newstate = getState();

	if (oldstate != newstate) {
		if (newstate.code == State::ACTIVE)
			enterActive();
		else if (oldstate.code == State::ACTIVE)
			leaveActive();

		statechangedsig.emit(make_pair(oldstate, newstate));
	}

	if (newstate.code == State::ACTIVE)
		work(dt);
}

void Worker::work(double dt) { }

void Worker::enterActive() { }

void Worker::leaveActive() { }

