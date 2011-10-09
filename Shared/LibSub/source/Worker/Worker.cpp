#include "LibSub/Worker/Worker.h"

using namespace subjugator;
using namespace boost;

Worker::Worker() : curstate(State::STANDBY) { }

void Worker::update(double dt) {
	State oldstate = curstate;
	curstate = getUpdatedState(dt);
	if (oldstate != curstate)
		statechangedsig(oldstate, curstate);
	
	if (curstate.code == State::ACTIVE)
		work(dt);
}

Worker::State Worker::getUpdatedState(double dt) const {
	State newstate = State::ACTIVE;
	for (UpdaterVec::const_iterator i = updatervec.begin(); i != updatervec.end(); ++i) {
		State state = (*i)->updateState(dt);
		
		if (newstate.code > state.code) {
			state = newstate;
		} else if (newstate.code == state.code) {
			if (newstate.msg.size())
				state.msg += " " + newstate.msg;
		}
	}
	
	return newstate;
}

