#ifndef SubDVLWorker_H
#define SubDVLWorker_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/SubWorker.h"
#include "HAL/SubHAL.h"
#include "DataObjects/DVL/DVLDataObjectFormatter.h"
#include "DataObjects/DVL/DVLCommand.h"
#include "DataObjects/DVL/DVLConfiguration.h"
#include "DataObjects/DVL/DVLBreak.h"
#include "DataObjects/DVL/DVLPacketFormatter.h"

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace subjugator
{
	class DVLWorker : public Worker
	{
		public:
			DVLWorker(boost::asio::io_service& io, int64_t rate);
			~DVLWorker()
			{
				if(pEndpoint)
					delete pEndpoint;
			}

			bool Startup();
			void Shutdown();

		private:
			SubHAL hal;
			DataObjectEndpoint* pEndpoint;

			void readyState();
			void emergencyState();
			void failState();
			void halReceiveCallback(std::auto_ptr<DataObject> &dobj);
			void halStateChangeCallback();
	};
}


#endif // SubDVLWorker_H

