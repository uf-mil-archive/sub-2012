#include "WorkerManager/WorkerManagerList.h"
#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <memory>

using namespace subjugator;
using namespace boost;
using namespace boost::algorithm;
using namespace std;

WorkerManagerList::WorkerManagerList(Callbacks &callbacks) : callbacks(callbacks) { }

void WorkerManagerList::addWorker(const string &name, const vector<string> &args, const WorkerManager::Options &opts) {
	auto_ptr<WorkerManager> pm(new WorkerManager(name, args, opts));
	manmap.insert(name, pm);
}

WorkerManager *WorkerManagerList::getWorkerManager(const std::string &name) const {
	ManMap::const_iterator i = manmap.find(name);
	if (i == manmap.end())
		return NULL;
	return const_cast<WorkerManager *>(i->second);
}

void WorkerManagerList::updateState(double dt) {
	state = State::ACTIVE;

	for (ManMap::iterator i = manmap.begin(); i != manmap.end(); ++i) {
		WorkerManager &wm = *i->second;

		Process::State prevprocstate = wm.getProcessState();
		wm.updateState(dt);
		Process::State curprocstate = wm.getProcessState();

		if (curprocstate != prevprocstate)
			callbacks.processStateChanged(wm);
		if (wm.getLastAction() != WorkerManager::NO_ACTION)
			callbacks.workerManagerAction(wm);

		state = state.combine(wm.getState());
	}
}


