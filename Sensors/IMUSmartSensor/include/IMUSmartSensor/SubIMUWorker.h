#ifndef SubIMUWorker_H
#define SubIMUWorker_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/SubWorker.h"
#include "HAL/IOThread.h"
#include "HAL/SubHAL.h"
#include "HAL/format/SPIPacketFormatter.h"
#include "DataObjects/IMU/IMUDataObjectFormatter.h"

namespace subjugator
{
	class IMUWorker : public Worker
	{
		public:
			IMUWorker(boost::asio::io_service& io, int64_t rate);
			~IMUWorker(){}

			bool Startup();

		private:
			IOThread iothread;
			SubHAL hal;
			std::auto_ptr<DataObjectEndpoint> pEndpoint;

			void readyState();
			void emergencyState();
			void failState();
			void halReceiveCallback(std::auto_ptr<DataObject> &dobj);
			void halStateChangeCallback();
	};
}


#endif // SubIMUWorker_H

