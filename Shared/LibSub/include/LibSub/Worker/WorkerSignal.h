#ifndef LIBSUB_WORKER_WORKERSIGNAL_H
#define LIBSUB_WORKER_WORKERSIGNAL_H

#include "LibSub/Worker/Worker.h"
#include <boost/optional.hpp>
#include <boost/signals2.hpp>
#include <limits>

namespace subjugator {
	template <typename T>
	class WorkerSignal {
		typedef boost::signals2::signal<void (const T&)> Signal;
		
		public:
			typedef boost::signals2::connection Connection;
		
			Connection connect(const typename Signal::slot_type &subscriber) {
				return signal.connect(subscriber);
			}
			
			const boost::optional<T> &getData() const { return data; }
			
			void emit(const T &newdata) {
				data = newdata;
				signal(newdata);
			}
			
		private:
			Signal signal;
			boost::optional<T> data;
	};
}

#endif

