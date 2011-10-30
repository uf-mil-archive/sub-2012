#include "LibSub/Worker/WorkerEndpoint.h"

using namespace subjugator;
using namespace boost;

WorkerEndpoint::WorkerEndpoint(DataObjectEndpoint *endpoint, const std::string &name, const InitializeCallback &initcallback, bool outgoingonly, double maxage, const Callback &callback)
: WorkerMailbox<boost::shared_ptr<DataObject> >(name, maxage, callback),
  endpoint(endpoint),
  initcallback(initcallback),
  outgoingonly(outgoingonly),
  errorage(0),
  state(WorkerState::STANDBY) {
	endpoint->configureCallbacks(boost::bind(&WorkerEndpoint::halReceiveCallback, this, _1), boost::bind(&WorkerEndpoint::halStateChangeCallback, this));
	endpoint->open();
}

void WorkerEndpoint::updateState(double dt) {
	WorkerMailbox<boost::shared_ptr<DataObject> >::updateState(dt);

	switch (endpoint->getState()) {
		case Endpoint::OPEN:
			if (outgoingonly) // if we're only concerned with the ability to send messages (outgoingonly)
				state = WorkerState::ACTIVE; // then having an open endpoint is enough to consider us active
			else
				state = WorkerMailbox<boost::shared_ptr<DataObject> >::getWorkerState();
			break;

		case Endpoint::CLOSED:
			state = WorkerState(WorkerState::STANDBY, "Waiting for " + getName() + " endpoint to open");
			break;

		default:
		case Endpoint::ERROR:
			state = WorkerState(WorkerState::ERROR, "Error with " + getName() + " endpoint: " + endpoint->getErrorMessage());
			break;
	}

	if (state.code == WorkerState::STANDBY || state.code == WorkerState::ERROR) {
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
	set(shared_ptr<DataObject>(dobj));
}

void WorkerEndpoint::halStateChangeCallback() {
	if (endpoint->getState() == Endpoint::OPEN && initcallback)
		initcallback(*endpoint);
	// else if (endpoint->getState() == Endpoint::ERROR) // TODO, Worker-level logging system would come in to play here somehow?
}

