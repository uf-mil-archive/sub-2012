#ifndef HYDROPHONE_FTD2XXENDPOINT_H
#define HYDROPHONE_FTD2XXENDPOINT_H

#include "Hydrophone/ftd2xx/FTD2XX.h"
#include "HAL/transport/BaseEndpoint.h"
#include <boost/thread.hpp>
#include <string>

namespace subjugator {
	class FTD2XXEndpoint : public BaseEndpoint {
		public:
			FTD2XXEndpoint(int devnum);

			virtual void open();
			virtual void close();
			virtual void write(ByteVec::const_iterator begin, ByteVec::const_iterator end);

		private:
			int devnum;
			FTD2XX ftd;

			boost::thread readthread;
			void readthread_run();

			boost::thread writethread;
			void writethread_run();
			ByteVec writebuf;
			boost::mutex writemutex;
			boost::condition_variable writecond;
	};
}

#endif

