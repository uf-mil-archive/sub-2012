#ifndef LIBSUB_WORKER_WORKERSIGNAL_H
#define LIBSUB_WORKER_WORKERSIGNAL_H

#include <boost/optional.hpp>
#include <boost/signals2.hpp>
#include <limits>

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	\brief An output signal for Workers

	Simple wrapper for boost::signals2, which also retains the most recently sent
	data to allow for polling in addition to signal callbacks.
	*/

	template <typename T>
	class WorkerSignal {
		typedef boost::signals2::signal<void (const T&)> Signal;

		public:
			typedef boost::signals2::connection Connection;

			/**
			\brief Connects a callback to the signal
			
			Adds a callback to the signal, and returns a Connection object.
			When the Connection object is destroyed, the callback is removed from the Signal.
			*/

			Connection connect(const typename Signal::slot_type &subscriber) {
				return signal.connect(subscriber);
			}

			/**
			\brief Gets the most recently emited object
			*/

			const boost::optional<T> &getData() const { return data; }

			/**
			\brief Emits data on the signal
			*/
			
			void emit(const T &newdata) {
				data = newdata;
				signal(newdata);
			}

		private:
			Signal signal;
			boost::optional<T> data;
	};

	/** @} */
}

#endif

