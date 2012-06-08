#ifndef WORKERMANAGER_WORKERMANAGERLIST_H
#define WORKERMANAGER_WORKERMANAGERLIST_H

#include "WorkerManager/WorkerManager.h"
#include <boost/ptr_container/ptr_map.hpp>
#include <vector>
#include <string>

namespace subjugator {
	class WorkerManagerList : public StateUpdater {
		public:
			struct Callbacks {
				virtual void processStateChanged(WorkerManager &wm)=0;
				virtual void workerManagerAction(WorkerManager &wm)=0;
			};

			WorkerManagerList(Callbacks &callbacks);
			void addWorker(const std::string &name, const std::string &procname, const std::vector<std::string> &args, const WorkerManager::Options &opts);

			WorkerManager *getWorkerManager(const std::string &name) const;

			virtual const State &getState() const { return state; }
			virtual void updateState(double dt);

		private:
			Callbacks &callbacks;

			typedef boost::ptr_map<std::string, WorkerManager> ManMap;
			ManMap manmap;

			State state;
	};
}

#endif

