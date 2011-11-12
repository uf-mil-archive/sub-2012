#include "LibSub/Worker/Worker.h"
#include <boost/lexical_cast.hpp>

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

		statechangedsig.emit(newstate);

		string logmsg = "State changed to " + lexical_cast<string>(newstate);
		logger.log(logmsg, newstate.code == State::ERROR ? WorkerLogEntry::ERROR : WorkerLogEntry::INFO);
	}

	if (newstate.code == State::ACTIVE)
		work(dt);
}

void Worker::work(double dt) { }

void Worker::enterActive() { }

void Worker::leaveActive() { }

