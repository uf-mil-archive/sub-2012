#include "LibSub/Worker/Worker.h"
#include <boost/lexical_cast.hpp>

using namespace subjugator;
using namespace boost;
using namespace boost::property_tree;
using namespace std;

Worker::Worker(const std::string &name, double updatehz, const WorkerConfigLoader &configloader) :
logger(name),
name(name),
updatehz(updatehz),
configloader(configloader),
initialized(false) { }

void Worker::update(double dt) {
	if (!initialized) {
		initialize();
		initialized = true;
	}
	
	runtime += dt;

	State oldstate = getState();
	updateState(dt);
	const State &newstate = getState();
	
	assert(newstate.code != State::UNINITIALIZED);

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

void Worker::initialize() { }

void Worker::work(double dt) { }

void Worker::enterActive() { }

void Worker::leaveActive() { }

const ptree &Worker::getConfig() const {
	if (!configcache)
		configcache = configloader.loadConfig(getName());
	return *configcache;
}
