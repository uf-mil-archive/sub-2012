#ifndef LIBSUB_WORKER_WORKERSTATEUPDATER_H
#define LIBSUB_WORKER_WORKERSTATEUPDATER_H

#include "LibSub/Worker/WorkerState.h"
#include <boost/noncopyable.hpp>
#include <vector>

namespace subjugator {
	class WorkerStateUpdater {
		public:
			virtual const WorkerState &getWorkerState() const = 0;
			virtual void updateState(double dt) = 0;
	};
	
	class WorkerStateUpdaterContainer : public WorkerStateUpdater, boost::noncopyable {
		typedef std::vector<WorkerStateUpdater *> UpdaterVec;
	
		public:
			virtual const WorkerState &getWorkerState() const;
			virtual void updateState(double dt);
			
		protected:
			void registerStateUpdater(WorkerStateUpdater &updater) { updatervec.push_back(&updater); }
		
		private:
			UpdaterVec updatervec;
			WorkerState state;
	};
}

#endif

