#ifndef LIBSUB_WORKER_WORKERDATA_H
#define LIBSUB_WORKER_WORKERDATA_H

#include "LibSub/Worker/Worker.h"
#include <boost/optional.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <limits>

namespace subjugator {
	template <typename T>
	class WorkerData : public Worker::StateUpdater {
		public:
			WorkerData(const std::string &name, double maxage=std::numeric_limits<double>::infinity())
			: name(name), maxage(maxage), age(0), datataken(false) { }
		
			void set(const T& newdata) {
				boost::lock_guard<boost::mutex> lock(mutex);
				datataken = false;
				age = 0;
				data.reset(newdata);
			}
			
			void clear() {
				boost::lock_guard<boost::mutex> lock(mutex);
				datataken = false;
				age = 0;
				data.reset();
			}
			
			boost::optional<T> get() const {
				boost::lock_guard<boost::mutex> lock(mutex);
				return data;
			}
			
			boost::optional<T> take() {
				boost::lock_guard<boost::mutex> lock(mutex);
				if (datataken)
					return boost::optional<T>();
				
				datataken = true;
				return data;
			}
			
			virtual Worker::State updateState(double dt) {
				boost::lock_guard<boost::mutex> lock(mutex);
				if (data) {
					age += dt;
					if (age < maxage)
						return Worker::State::ACTIVE;
					else
						return Worker::State(Worker::State::STANDBY, "Stale " + name + " data");
				} else {
					return Worker::State(Worker::State::STANDBY, "Waiting for " + name + " data");
				}
			}
			
		protected:
			std::string name;
			
		private:
			double maxage;
			double age;
		
			boost::optional<T> data;
			bool datataken;
			
			mutable boost::mutex mutex;
	};
}

#endif

