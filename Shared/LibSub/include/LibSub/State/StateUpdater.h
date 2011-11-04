#ifndef LIBSUB_STATE_STATEUPDATER_H
#define LIBSUB_STATE_STATEUPDATER_H

#include "LibSub/State/State.h"
#include <boost/noncopyable.hpp>
#include <vector>

namespace subjugator {
	class StateUpdater {
		public:
			virtual const State &getState() const = 0;
			virtual void updateState(double dt) = 0;
	};

	class StateUpdaterContainer : public StateUpdater, boost::noncopyable {
		typedef std::vector<StateUpdater *> UpdaterVec;

		public:
			virtual const State &getState() const;
			virtual void updateState(double dt);

		protected:
			void registerStateUpdater(StateUpdater &updater) { updatervec.push_back(&updater); }

		private:
			UpdaterVec updatervec;
			State state;
	};
}

#endif

