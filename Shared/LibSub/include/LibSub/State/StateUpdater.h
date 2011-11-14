#ifndef LIBSUB_STATE_STATEUPDATER_H
#define LIBSUB_STATE_STATEUPDATER_H

#include "LibSub/State/State.h"
#include <boost/noncopyable.hpp>
#include <vector>

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	StateUpdater is an interface for classes which have a State that varies over time.
	*/

	class StateUpdater {
		public:
			/** Gets the current state of the object */
			virtual const State &getState() const = 0;
			/** Updates the state of the object assuming dt seconds have passed */
			virtual void updateState(double dt) = 0;
	};

	/**
	StateUpdaterContainer is a StateUpdater whose state comes from a combination of other StateUpdaters.
	Used as a base class for Workers, among other things.
	*/

	class StateUpdaterContainer : public StateUpdater, boost::noncopyable {
		typedef std::vector<StateUpdater *> UpdaterVec;

		public:
			virtual const State &getState() const;
			virtual void updateState(double dt);

		protected:
			/**
			Adds updater to the list of StateUpdaters which the StateUpdaterContainer derives its state from.
			The StateUpdaterContainer does not take ownership of the updater.
			*/
			void registerStateUpdater(StateUpdater &updater) { updatervec.push_back(&updater); }

		private:
			UpdaterVec updatervec;
			State state;
	};

	/** @} */
}

#endif

