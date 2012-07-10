#ifndef HYDROPHONE_HYDROPHONEWORKER_H
#define HYDROPHONE_HYDROPHONEWORKER_H

#include "Hydrophone/HydrophoneDataProcessor.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "HAL/HAL.h"

namespace subjugator {
	class HydrophoneWorker : public Worker {
		public:
			struct Info {
				boost::uint64_t timestamp;
				double distance;
				double heading;
				double declination;
				double pingfrequency;
				bool valid;
			};

			HydrophoneWorker(HAL &hal, const WorkerConfigLoader &configloader);

			WorkerSignal<Info> signal;

		private:
			HAL &hal;
			WorkerEndpoint endpoint;

			HydrophoneDataProcessor::Config dpconfig;

			void endpointInitCallback();
			void endpointReceiveCallback(const boost::shared_ptr<DataObject> &dobj);
	};
}

#endif /* SUBHYDROPHONEWORKER_H_ */
