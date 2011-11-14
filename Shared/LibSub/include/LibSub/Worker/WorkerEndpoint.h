#ifndef LIBSUB_WORKER_WORKERENDPOINT_H
#define LIBSUB_WORKER_WORKERENDPOINT_H

#include "LibSub/State/StateUpdater.h"
#include "HAL/format/DataObjectEndpoint.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	\brief Worker utility for managing a HAL Endpoint

	WorkerEndpoint is a utility class for managing a HAL DataObjectEndpoint. It is a StateUpdater,
	and implements error recovery by attempting to reconnect when an endpoint encounters an error,
	or when it stops receiving data.

	It presents a flexible interface to the worker, supporting both polling through getDataObject(),
	and a callback passed in the constructor.
	*/

	class WorkerEndpoint : public StateUpdater {
		public:
			typedef boost::function<void (DataObjectEndpoint&)> InitializeCallback;
			typedef boost::function<void (const boost::shared_ptr<DataObject> &)> ReceiveCallback;

			/**
			Constructs the WorkerEndpoint.

			\arg \c endpoint        The DataObjectEndpoint to be managed
			\arg \c name            A name that appears in log messages
			\arg \c initcallback    Called to initialize the endpoint on startup and to attempt error recovery
			\arg \c outgoingonly    A flag indicating that the WorkerEndpoint should not
			                        expect data to continually arrive from the Endpoint
			\arg \c maxdobjage      The amount of time in seconds before the WorkerEndpoint considers
			                        a DataObject stale and atempts to re-initialize the endpoint
			\arg \c receivecallback Called whenever a DataObject is received
			*/

			WorkerEndpoint(DataObjectEndpoint *endpoint,
			               const std::string &name,
			               const InitializeCallback &initcallback = InitializeCallback(),
			               bool outgoingonly = false,
			               double maxdobjage = std::numeric_limits<double>::infinity(),
			               const ReceiveCallback &receivecallback = ReceiveCallback());

			DataObjectEndpoint &getEndpoint() { return *endpoint; }
			const DataObjectEndpoint &getEndpoint() const { return *endpoint; }

			boost::shared_ptr<DataObject> get() const { return dobj; }
			void write(const DataObject &dobj) { return endpoint->write(dobj); }

			/**
			\brief Retrieves a specific DataObject subclass

			Uses a dynamic cast to attempt to convert the most recently received DataObject to type T.
			Returns an empty shared_ptr on cast failure.
			*/
			template <typename T> boost::shared_ptr<T> getDataObject() const {
				return boost::dynamic_pointer_cast<T>(get());
			}

			virtual const State &getState() const { return state; }
			virtual void updateState(double dt);

		private:
			boost::scoped_ptr<DataObjectEndpoint> endpoint;
			std::string name;
			InitializeCallback initcallback;
			bool outgoingonly;
			double maxdobjage;
			ReceiveCallback receivecallback;

			boost::shared_ptr<DataObject> dobj;
			double dobjage;
			double errorage;
			State state;

			void halReceiveCallback(std::auto_ptr<DataObject> &dobj);
			void halStateChangeCallback();
	};

	/** @} */
}

#endif

