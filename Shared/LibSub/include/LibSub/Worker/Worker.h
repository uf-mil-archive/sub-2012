#ifndef LIBSUB_WORKER_WORKER_H
#define LIBSUB_WORKER_WORKER_H

#include "LibSub/State/State.h"
#include "LibSub/State/StateUpdater.h"
#include "LibSub/Worker/WorkerSignal.h"
#include "LibSub/Worker/WorkerLogger.h"
#include <utility>

namespace subjugator {
	class Worker : protected StateUpdaterContainer {
		public:
			Worker(const std::string &name, double updatehz) : logger(name), name(name), updatehz(updatehz) { }

			const std::string &getName() const { return name; }
			double getUpdateHz() const { return updatehz; }

			WorkerSignal<std::pair<State, State> > statechangedsig;
			WorkerLogger logger;

			const State &getState() const { return StateUpdaterContainer::getState(); }
			bool isActive() const { return getState().code == State::ACTIVE; }

			void update(double dt);

		protected:
			virtual void work(double dt);

			virtual void enterActive();
			virtual void leaveActive();

		private:
			std::string name;
			double updatehz;
	};
}

#endif

