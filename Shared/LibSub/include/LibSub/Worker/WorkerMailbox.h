#ifndef LIBSUB_WORKER_WORKERMAILBOX_H
#define LIBSUB_WORKER_WORKERMAILBOX_H

#include "LibSub/State/StateUpdater.h"
#include <boost/optional.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/pointer_cast.hpp>
#include <limits>

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	\brief Input for a Worker

	Receives incoming data for a Worker. Keeps track of how old the data is, and provides
	several different access methods, in addition to acting as a StateUpdater.
	*/

	template <typename T>
	class WorkerMailbox : public StateUpdater {
		public:
			typedef boost::function<void (const boost::optional<T> &) > Callback;

			/**
			Configures and initializes a WorkerMailbox

			\arg \c name A name for the WorkerMailbox, which appears in the State messages
			\arg \c maxage The maximum age of data in the mailbox, in seconds
			\arg \c callback Called when new data is set
			*/
			WorkerMailbox(const std::string &name, double maxage=std::numeric_limits<double>::infinity(), const Callback &callback=Callback())
			: name(name), callback(callback), maxage(maxage), age(0), datataken(false) { }

			const std::string &getName() const { return name; }

			bool hasData() const { return (bool)data; }

			/**
			\brief Sets new data in the Mailbox
			*/

			void set(const T& newdata) {
				datataken = false;
				age = 0;
				data.reset(newdata);

				if (callback)
					callback(data);
			}

			/**
			\brief Clears the data in Mailbox.

			Used to indicate when data is no longer valid, but no new data is available.
			*/

			void clear() {
				datataken = false;
				age = 0;
				data.reset();

				if (callback)
					callback(data);
			}

			/**
			\brief Gets the most recent data or a default value
			*/

			T get(const T &def=T()) const {
				return getOptional().get_value_or(def);
			}

			/**
			\brief Gets the most recent data
			*/

			const boost::optional<T> &getOptional() const {
				return data;
			}

			/**
			\brief Takes the most recent data if it has not yet been taken, or returns a default value
			*/

			T take(const T &def=T()) {
				return takeOptional().get_value_or(def);
			}

			/**
			\brief Takes the most recent data if it has not yet been taken
			*/

			boost::optional<T> takeOptional() {
				if (datataken)
					return boost::optional<T>();

				datataken = true;
				return data;
			}

			virtual const State &getState() const { return state; }

			virtual void updateState(double dt) {
				if (data) {
					age += dt;
					if (age < maxage)
						state = State::ACTIVE;
					else
						state = State(State::STANDBY, "Stale " + name + " data");
				} else {
					state = State(State::STANDBY, "Waiting for " + name + " data");
				}
			}

		private:
			std::string name;
			Callback callback;
			double maxage;
			double age;

			boost::optional<T> data;
			bool datataken;

			State state;
	};

	/** @} */
}

#endif

