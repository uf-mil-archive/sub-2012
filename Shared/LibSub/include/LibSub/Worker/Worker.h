#ifndef LIBSUB_WORKER_WORKER_H
#define LIBSUB_WORKER_WORKER_H

#include "LibSub/State/State.h"
#include "LibSub/State/StateUpdater.h"
#include "LibSub/Worker/WorkerSignal.h"
#include <utility>

namespace subjugator {
	class Worker : protected StateUpdaterContainer {
		public:
			struct Properties {
				std::string name;
				double updatehz;
			};
			virtual const Properties &getProperties() const = 0;

			WorkerSignal<std::pair<State, State> > statechangedsig;
			const State &getState() const { return StateUpdaterContainer::getState(); }
			bool isActive() const { return getState().code == State::ACTIVE; }

			void update(double dt);

		protected:
			virtual void work(double dt);

			virtual void enterActive();
			virtual void leaveActive();
	};
}

#endif

