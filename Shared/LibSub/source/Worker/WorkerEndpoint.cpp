#include "LibSub/Worker/WorkerEndpoint.h"
#include <boost/bind.hpp>
#include <cassert>

using namespace subjugator;
using namespace boost;

WorkerEndpoint::WorkerEndpoint(const Args &args) :
Args(args),
dobjage(0),
errorage(0) {
	assertValidArgs();
	endpoint->configureCallbacks(boost::bind(&WorkerEndpoint::halReceiveCallback, this, _1), boost::bind(&WorkerEndpoint::halStateChangeCallback, this));
	endpoint->open();
}

void WorkerEndpointArgs::assertValidArgs() {
	assert(name.size());
	assert(endpoint);
}

void WorkerEndpoint::updateState(double dt) {
	if (dobj)
		dobjage += dt;
	else
		dobjage = 0;

	switch (endpoint->getState()) {
		case Endpoint::OPEN:
			if (outgoingonly) // if we're only concerned with the ability to send messages (outgoingonly)
				state = State::ACTIVE; // then having an open endpoint is enough to consider us active
			else if (dobj && dobjage < maxage)
				state = State::ACTIVE;
			else if (!dobj)
				state = State(State::STANDBY, "Waiting for data on " + name + " endpoint");
			else
				state = State(State::ERROR, "Stale data on " + name + " endpoint");
			break;

		case Endpoint::CLOSED:
			state = State(State::STANDBY, "Waiting for " + name + " endpoint to open");
			break;

		default:
		case Endpoint::ERROR:
			state = State(State::ERROR, "Error with " + name + " endpoint: " + endpoint->getErrorMessage());
			break;
	}

	if (state.code == State::STANDBY || state.code == State::ERROR) {
		errorage += dt;
		if (errorage >= 1) {
			errorage = 0;
			endpoint->close();
			endpoint->open();
		}
	} else {
		errorage = 0;
	}
}

void WorkerEndpoint::halReceiveCallback(std::auto_ptr<DataObject> &dobj) {
	this->dobj = shared_ptr<DataObject>(dobj);
	dobjage = 0;
}

void WorkerEndpoint::halStateChangeCallback() {
	if (endpoint->getState() == Endpoint::OPEN && initcallback)
		initcallback(*endpoint);
	// else if (endpoint->getState() == Endpoint::ERROR) // TODO, Worker-level logging system would come in to play here somehow?
}

