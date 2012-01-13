#ifndef IMU_IMUWORKER_H
#define IMU_IMUWORKER_H

#include "IMU/DataObjects/IMUInfo.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "HAL/HAL.h"

namespace subjugator {
	class IMUWorker : public Worker {
		public:
			IMUWorker(HAL &hal, const WorkerConfigLoader &configloader);
			
			WorkerSignal<IMUInfo> signal;

		private:
			HAL &hal;
			WorkerEndpoint endpoint;

			void endpointReceiveCallback(const boost::shared_ptr<DataObject> &dobj);
	};
}


#endif // SubIMUWorker_H

