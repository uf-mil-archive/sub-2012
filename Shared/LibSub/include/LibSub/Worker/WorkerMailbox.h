#ifndef LIBSUB_WORKER_WORKERMAILBOX_H
#define LIBSUB_WORKER_WORKERMAILBOX_H

#include "LibSub/Worker/WorkerStateUpdater.h"
#include <boost/optional.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/pointer_cast.hpp>
#include <limits>

namespace subjugator {
	template <typename T>
	class WorkerMailbox : public WorkerStateUpdater {
		public:
			typedef boost::function<void (const boost::optional<T> &) > Callback;

			WorkerMailbox(const std::string &name, double maxage=std::numeric_limits<double>::infinity(), const Callback &callback=Callback())
			: name(name), callback(callback), maxage(maxage), age(0), datataken(false) { }

			const std::string &getName() const { return name; }

			bool hasData() const { return (bool)data; }

			void set(const T& newdata) {
				boost::lock_guard<boost::mutex> lock(mutex);
				datataken = false;
				age = 0;
				data.reset(newdata);

				if (callback)
					callback(*data);
			}

			void clear() {
				boost::lock_guard<boost::mutex> lock(mutex);
				datataken = false;
				age = 0;
				data.reset();

				if (callback)
					callback(*data);
			}

			T get(const T &def=T()) const {
				return getOptional().get_value_or(def);
			}

			boost::optional<T> getOptional() const {
				boost::lock_guard<boost::mutex> lock(mutex);
				return data;
			}

			T take(const T &def=T()) {
				return takeOptional().get_value_or(def);
			}

			boost::optional<T> takeOptional() {
				boost::lock_guard<boost::mutex> lock(mutex);
				if (datataken)
					return boost::optional<T>();

				datataken = true;
				return data;
			}

			virtual const WorkerState &getWorkerState() const { return state; }

			virtual void updateState(double dt) {
				boost::lock_guard<boost::mutex> lock(mutex);
				if (data) {
					age += dt;
					if (age < maxage)
						state = WorkerState::ACTIVE;
					else
						state = WorkerState(WorkerState::STANDBY, "Stale " + name + " data");
				} else {
					state = WorkerState(WorkerState::STANDBY, "Waiting for " + name + " data");
				}
			}

		private:
			std::string name;
			Callback callback;
			double maxage;
			double age;

			boost::optional<T> data;
			bool datataken;

			WorkerState state;

			mutable boost::mutex mutex;
	};
}

#endif

