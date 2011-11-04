#ifndef LIBSUB_WORKER_WORKERENDPOINT_H
#define LIBSUB_WORKER_WORKERENDPOINT_H

#include "LibSub/State/StateUpdater.h"
#include "HAL/format/DataObjectEndpoint.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace subjugator {
	class WorkerEndpoint : public StateUpdater {
		public:
			typedef boost::function<void (DataObjectEndpoint&)> InitializeCallback;
			typedef boost::function<void (const boost::shared_ptr<DataObject> &)> ReceiveCallback;

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
}

#endif

