#ifndef LIBSUB_WORKER_WORKERKILLMONITOR_H
#define LIBSUB_WORKER_WORKERKILLMONITOR_H

#include "LibSub/State/StateUpdater.h"
#include "LibSub/Worker/WorkerMap.h"
#include "LibSub/Worker/WorkerSignal.h"
#include <boost/optional.hpp>
#include <string>

namespace subjugator {
	struct WorkerKill {
		std::string name;
		std::string desc;
		bool killed;

		WorkerKill() { }
		WorkerKill(const std::string &name, const std::string &desc, bool killed=false) :
		name(name), desc(desc), killed(killed) { }

		std::string getName() const { return name; }
	};

	class WorkerKillSignal : public WorkerSignal<WorkerKill> {
		public:
			WorkerKillSignal(const std::string &killname, const std::string &desc);

			void setKill(bool kill);
			void kill() { setKill(true); }
			void unkill() { setKill(false); }

			bool isKilled() const { return getData() ? getData()->killed : false; }

		private:
			std::string killname;
			std::string desc;
	};

	class WorkerKillMonitor : public StateUpdater, public WorkerMap<std::string, WorkerKill> {
		public:
			WorkerKillMonitor(const std::string &selfkillname="");

			boost::optional<const WorkerKill &> getKill() const;
			bool isKilled() const { return getKill(); }

			virtual const State &getState() const { return state; }
			virtual void updateState(double dt);

		private:
			void update();

			std::string selfkillname;
			State state;
	};
}

#endif

