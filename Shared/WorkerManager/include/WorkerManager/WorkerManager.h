#ifndef WORKERMANAGER_WORKERMANAGER_H
#define WORKERMANAGER_WORKERMANAGER_H

#include "WorkerManager/Process.h"
#include "LibSub/State/StateUpdater.h"
#include <limits>

namespace subjugator {
	class WorkerManager : public StateUpdater {
		public:
			struct Options {
				bool exitrestart;
				double standbyrestarttime;
				double errorrestarttime;
				double stopkilltime;
			};

			enum Action {
				NO_ACTION,
				KILL_STOP,
				RESTART_EXIT,
				RESTART_STANDBY,
				RESTART_ERROR
			};

			WorkerManager(const std::string &name, const std::string &procname, const std::vector<std::string> &args, const Options &opts);

			const std::string &getName() const { return name; }

			void updateWorkerState(const State &workerstate) { this->workerstate = workerstate; }

			void start();
			void restart();
			void stop();

			bool isStarted() const { return started; }
			Process::State getProcessState() const { return proc.getState(); }
			const std::string &getProcessTerminationReason() const { return proc.getTerminationReason(); }
			Action getLastAction() const { return lastaction; }

			virtual const State &getState() const { return state; }
			virtual void updateState(double dt);

		private:
			std::string name;
			Options opts;
			Process proc;
			State state;
			Action lastaction;

			bool started;
			bool restarting;
			double stoptimer;
			double standbytimer;
			double errortimer;
			State workerstate;
	};
}

#endif

