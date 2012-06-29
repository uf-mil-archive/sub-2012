#ifndef WORKERMANAGER_WORKERMANAGERLIST_H
#define WORKERMANAGER_WORKERMANAGERLIST_H

#include "WorkerManager/WorkerManager.h"
#include <boost/ptr_container/ptr_map.hpp>
#include <vector>
#include <string>

namespace subjugator {
	class WorkerManagerList : public StateUpdater {
		typedef boost::ptr_map<std::string, WorkerManager> ManMap;

		public:
			struct Callbacks {
				virtual void processStateChanged(WorkerManager &wm)=0;
				virtual void workerManagerAction(WorkerManager &wm)=0;
			};

			WorkerManagerList(Callbacks &callbacks);
			void addWorker(const std::string &name, const std::vector<std::string> &args, const WorkerManager::Options &opts);

			WorkerManager *getWorkerManager(const std::string &name) const;

			typedef ManMap::const_iterator const_iterator;
			const_iterator begin() const { return manmap.begin(); }
			const_iterator end() const { return manmap.end(); }

			virtual const State &getState() const { return state; }
			virtual void updateState(double dt);

		private:
			Callbacks &callbacks;


			ManMap manmap;

			State state;
	};
}

#endif
