#include "LibSub/Worker/WorkerEndpoint.h"

using namespace subjugator;

WorkerEndpoint::WorkerEndpoint(DataObjectEndpoint *endpoint, const std::string &name, double maxage)
: WorkerData<std::auto_ptr<DataObject> >(name, maxage),
  endpoint(endpoint),
  errorage(0) { }

Worker::State WorkerEndpoint::updateState(double dt) {
	DataObjectEndpoint::State estate = endpoint->getState();
	
	if (estate == Endpoint::OPEN) {
		errorage = 0;
		reopening = false;
		return WorkerData::updateState(dt);
	}
	
	clear();
	
	if (estate == Endpoint::CLOSED)
		return Worker::State(reopening ? Worker::State::ERROR : Worker::State::STANDBY, "Waiting for " + name + " endpoint to open");
	
	errorage += dt;
	if (errorage >= 1) {
		errorage = 0;
		endpoint->close();
		endpoint->open();
		reopening = true;
	}
	
	return Worker::State(Worker::State::ERROR, "Error with " + name + " endpoint: " + endpoint->getErrorMessage());
}

