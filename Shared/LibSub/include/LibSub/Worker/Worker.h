#ifndef LIBSUB_WORKER_WORKER_H
#define LIBSUB_WORKER_WORKER_H

#include "LibSub/Worker/WorkerState.h"
#include "LibSub/Worker/WorkerStateUpdater.h"
#include "LibSub/Worker/WorkerSignal.h"
#include <utility>

namespace subjugator {
	class Worker : protected WorkerStateUpdaterContainer {
		public:
			struct Properties {
				std::string name;
				double updatehz;
			};
			virtual const Properties &getProperties() const = 0;
		
			WorkerSignal<std::pair<WorkerState, WorkerState> > statechangedsig;
			const WorkerState &getState() const { return getWorkerState(); }
			bool isActive() const { return getState().code == WorkerState::ACTIVE; }
			
			void update(double dt);
			
		protected:
			virtual void work(double dt);
			
			virtual void enterActive();
			virtual void leaveActive();
	};
}

#endif

