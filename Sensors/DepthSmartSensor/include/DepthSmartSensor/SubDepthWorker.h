#ifndef SubDepthWorker_H
#define SubDepthWorker_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/SubWorker.h"
#include "HAL/HAL.h"
#include "HAL/SubHAL.h"
#include "HAL/IOThread.h"
#include "DataObjects/Depth/DepthDataObjectFormatter.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include "DataObjects/Embedded/StopPublishing.h"

namespace subjugator
{
	class DepthWorker : public Worker
	{
		public:
			DepthWorker(boost::asio::io_service& io, int64_t rate);
			~DepthWorker()
			{
				if(pEndpoint)
					delete pEndpoint;
			}

			bool Startup();
			void Shutdown();

		private:
			IOThread iothread;
			SubHAL hal;
			DataObjectEndpoint* pEndpoint;

			void readyState();
			void emergencyState();
			void failState();
			void halReceiveCallback(std::auto_ptr<DataObject> &dobj);
			void halStateChangeCallback();
	};
}


#endif // SubDepthWorker_H

