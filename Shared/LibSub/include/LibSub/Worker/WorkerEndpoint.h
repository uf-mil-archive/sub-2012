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
	\brief WorkerEndpoint constructor arguments

	Helper object for constructing \c WorkerEndpoint, which allows for a named argument like syntax.
	Usually referred through the typedef \c WorkerEndpoint::Args.

	\arg \c endpoint        The DataObjectEndpoint to be managed
	\arg \c name            A name that appears in log messages
	\arg \c initcallback    Called to initialize the endpoint on startup and to attempt error recovery
	\arg \c outgoingonly    A flag indicating that the WorkerEndpoint should not
	                        expect data to continually arrive from the Endpoint
	\arg \c maxdobjage      The amount of time in seconds before the WorkerEndpoint considers
	                        a DataObject stale and atempts to re-initialize the endpoint
	\arg \c receivecallback Called whenever a DataObject is received

	\see WorkerEndpoint
	*/

	class WorkerEndpointArgs {
		public:
			typedef boost::function<void (DataObjectEndpoint&)> InitializeCallback;
			typedef boost::function<void (const boost::shared_ptr<DataObject> &)> ReceiveCallback;

			WorkerEndpointArgs() :
		    outgoingonly(false), maxage(std::numeric_limits<double>::infinity()) { }

			WorkerEndpointArgs &setEndpoint(DataObjectEndpoint *endpoint) {
				this->endpoint.reset(endpoint);
				return *this;
			}

			WorkerEndpointArgs &setName(const std::string &name) {
				this->name = name;
				return *this;
			}

			WorkerEndpointArgs &setInitCallback(const InitializeCallback &initcallback) {
				this->initcallback = initcallback;
				return *this;
			}

			WorkerEndpointArgs &setReceiveCallback(const ReceiveCallback &receivecallback) {
				this->receivecallback = receivecallback;
				return *this;
			}

			WorkerEndpointArgs &setOutgoingOnly(bool outgoingonly=true) {
				this->outgoingonly = outgoingonly;
				return *this;
			}

			WorkerEndpointArgs &setMaxAge(double maxage) {
				this->maxage = maxage;
				return *this;
			}

		protected:
			void assertValidArgs();

			boost::shared_ptr<DataObjectEndpoint> endpoint;
			std::string name;
			InitializeCallback initcallback;
			ReceiveCallback receivecallback;
			bool outgoingonly;
			double maxage;
	};

	/**
	\brief Worker utility for managing a HAL Endpoint

	WorkerEndpoint is a utility class for managing a HAL DataObjectEndpoint. It is a StateUpdater,
	and implements error recovery by attempting to reconnect when an endpoint encounters an error,
	or when it stops receiving data.

	It presents a flexible interface to the worker, supporting both polling through getDataObject(),
	and a callback passed in the constructor.
	*/

	class WorkerEndpoint : public StateUpdater, private WorkerEndpointArgs {
		public:
			typedef WorkerEndpointArgs Args;

			/**
			\brief Constructs WorkerEndpointArgs

			Uses \c WorkerEndpointArgs to implement a named parameter like syntax.

			\see WorkerEndpointArgs
			*/

			WorkerEndpoint(const Args &args);

			void clearMaxAge() { maxage = std::numeric_limits<double>::infinity(); }
			void setMaxAge(double maxage) { this->maxage = maxage; }

			DataObjectEndpoint &getEndpoint() { return *endpoint; }
			const DataObjectEndpoint &getEndpoint() const { return *endpoint; }
			const std::string &getName() { return name; }

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
