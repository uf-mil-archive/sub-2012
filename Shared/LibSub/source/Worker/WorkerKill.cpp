#include "LibSub/Worker/WorkerKill.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace std;

WorkerKillSignal::WorkerKillSignal(const std::string &killname, const std::string &desc) :
killname(killname),
desc(desc) { }

void WorkerKillSignal::setKill(bool kill) {
	if (getData() && getData()->killed == kill)
		return;

	emit(WorkerKill(killname, desc, kill));
}

WorkerKillMonitor::WorkerKillMonitor(const std::string &selfkillname, const KillChangedCallback &callback) :
	WorkerMap<std::string, WorkerKill>(WorkerMap<std::string, WorkerKill>::Args()
	                                   .setKeyCallback(&WorkerKill::getName)
	                                   .setUpdateCallback(bind(&WorkerKillMonitor::onUpdate, this, _1, _2))),
	callback(callback),
	selfkillname(selfkillname)
{ }

optional<const WorkerKill &> WorkerKillMonitor::getKill() const {
	for (WorkerMap<std::string, WorkerKill>::const_iterator i = begin(); i != end(); ++i) {
		const WorkerKill &wk = i->second;

		if (wk.killed && wk.name != selfkillname)
			return wk;
	}

	return none;
}

void WorkerKillMonitor::updateState(double dt) {
	optional<const WorkerKill &> wk = getKill();
	if (wk) {
		state = State(State::STANDBY, "Killed by " + wk->name);
	} else {
		state = State::ACTIVE;
	}
}

void WorkerKillMonitor::onUpdate(const optional<WorkerKill> &prev, const optional<WorkerKill> &cur) {
	if (!callback)
		return;

	bool prevkilled = prev && prev->killed;
	bool curkilled = cur && cur->killed;
	if (!prevkilled && curkilled) {
		bool onlykill = true;
		for (WorkerMap<std::string, WorkerKill>::const_iterator i = begin(); i != end(); ++i) {
			if (i->second.name != cur->name && i->second.killed && i->second.name != selfkillname) {
				onlykill = false;
				break;
			}
		}

		if (onlykill)
			callback();
	} else if (prevkilled && !curkilled) {
		if (!getKill())
			callback();
	}
}
