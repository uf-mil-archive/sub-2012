#ifndef MOTORCALIBRATE_HEARTBEATSENDER_H
#define MOTORCALIBRATE_HEARTBEATSENDER_H

#include "HAL/format/DataObjectEndpoint.h"
#include <boost/asio.hpp>

namespace subjugator {
	class HeartBeatSender {
		public:
			HeartBeatSender(boost::asio::io_service &io_service, DataObjectEndpoint &endpoint, double hz=1);

			void start();
			void stop();

		private:
			boost::asio::deadline_timer timer;
			DataObjectEndpoint &endpoint;
			double hz;

			void timerCallback(boost::system::error_code &error);
	};
}

#endif

