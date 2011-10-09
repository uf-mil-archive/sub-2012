#ifndef LIBSUB_WORKER_WORKERENDPOINT_H
#define LIBSUB_WORKER_WORKERENDPOINT_H

#include "LibSub/Worker/WorkerData.h"
#include "HAL/format/DataObjectEndpoint.h"
#include <boost/scoped_ptr.hpp>
#include <memory>

namespace subjugator {
	class WorkerEndpoint : public WorkerData<std::auto_ptr<DataObject> > {
		public:
			WorkerEndpoint(DataObjectEndpoint *endpoint, const std::string &name, double maxage=std::numeric_limits<double>::infinity());
			
			virtual Worker::State updateState(double dt);
			
		private:
			boost::scoped_ptr<DataObjectEndpoint> endpoint;
			double errorage;
			bool reopening;
	};
}

#endif

