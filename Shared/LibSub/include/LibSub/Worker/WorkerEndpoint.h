#ifndef LIBSUB_WORKER_WORKERENDPOINT_H
#define LIBSUB_WORKER_WORKERENDPOINT_H

#include "LibSub/Worker/WorkerMailbox.h"
#include "HAL/format/DataObjectEndpoint.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace subjugator {
	class WorkerEndpoint : public WorkerMailbox<boost::shared_ptr<DataObject> > {
		public:
			typedef boost::function<void (DataObjectEndpoint&)> InitializeCallback;

			WorkerEndpoint(DataObjectEndpoint *endpoint, const std::string &name, const InitializeCallback &initcallback=InitializeCallback(), bool outgoingonly=false, double maxage=std::numeric_limits<double>::infinity(), const Callback &callback=Callback());
			// ^^ TODO something other than that

			DataObjectEndpoint &getEndpoint() { return *endpoint; }
			const DataObjectEndpoint &getEndpoint() const { return *endpoint; }

			void write(const DataObject &dobj) { return endpoint->write(dobj); }

			template <typename T> boost::shared_ptr<T> getDataObject() const {
				return boost::dynamic_pointer_cast<T>(get());
			}

			virtual const WorkerState &getWorkerState() const { return state; }
			virtual void updateState(double dt);

		private:
			boost::scoped_ptr<DataObjectEndpoint> endpoint;
			InitializeCallback initcallback;
			bool outgoingonly;
			double errorage;
			WorkerState state;

			void halReceiveCallback(std::auto_ptr<DataObject> &dobj);
			void halStateChangeCallback();
	};
}

#endif

