#ifndef LIBSUB_WORKER_WORKER_H
#define LIBSUB_WORKER_WORKER_H

#include "LibSub/State/State.h"
#include "LibSub/State/StateUpdater.h"
#include "LibSub/Worker/WorkerSignal.h"
#include "LibSub/Worker/WorkerLogger.h"
#include "LibSub/Worker/WorkerConfigLoader.h"
#include <boost/optional.hpp>
#include <utility>

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	\brief Worker base class

	Worker is the base class for Workers, and contains common Worker functionality and properties.
	These include a state change signal, a logger, and a state
	*/

	class Worker : protected StateUpdaterContainer {
		public:
			Worker(const std::string &name, double updatehz, const WorkerConfigLoader &configloader);

			const std::string &getName() const { return name; }
			double getUpdateHz() const { return updatehz; }

			typedef WorkerSignal<State> StateChangedSignal;
			StateChangedSignal statechangedsig;
			WorkerLogger logger;

			const State &getState() const { return StateUpdaterContainer::getState(); }
			bool isActive() const { return getState().code == State::ACTIVE; }

			void update(double dt);

		protected:
			/**
			\brief Overridable method called once, just before work is called for the first time
			*/
			virtual void initialize();

			/**
			\brief Overridable method called at the workers update frequency only when the worker is in an active state.
			*/
			virtual void work(double dt);

			/**
			\brief Overridable method called when the worker changes into the active state.
			*/
			virtual void enterActive();

			/**
			\brief Overridable method called when the worker leaves the active state.
			*/
			virtual void leaveActive();

			double getRunTime() const { return runtime; }

		protected:
			const boost::property_tree::ptree &getConfig() const;
			void saveConfig(const boost::property_tree::ptree &config, bool useLocal=true) const { configloader.writeLocalConfig(name, config, useLocal); }

		private:
			std::string name;
			double updatehz;
			const WorkerConfigLoader &configloader;
			mutable boost::optional<boost::property_tree::ptree> configcache;
			
			bool initialized;
			double runtime;
	};

	/** @} */
}

#endif

