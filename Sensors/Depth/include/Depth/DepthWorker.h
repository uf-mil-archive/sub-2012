#ifndef DEPTH_DEPTHWORKER_H
#define DEPTH_DEPTHWORKER_H

#include "Depth/DataObjects/DepthInfo.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "HAL/HAL.h"

namespace subjugator {
	class DepthWorker : public Worker {
		public:
			DepthWorker(HAL &hal, const WorkerConfigLoader &configloader);

			WorkerSignal<DepthInfo> signal;

		private:
			HAL &hal;
			int publishrate;
			WorkerEndpoint endpoint;
			
			void endpointInitCallback();
			void endpointReceiveCallback(const boost::shared_ptr<DataObject> &dobj);
	};
}

#endif
