#include "LibSub/State/StateUpdater.h"
#include <boost/bind.hpp>
#include <algorithm>

using namespace subjugator;
using namespace std;

const State &StateUpdaterContainer::getState() const { return state; }

void StateUpdaterContainer::updateState(double dt) {
	state = State::ACTIVE;

	for (UpdaterVec::const_iterator i = updatervec.begin(); i != updatervec.end(); ++i) {
		StateUpdater &updater = **i;
		updater.updateState(dt);
		state = state.combine(updater.getState());
	}
}

