#ifndef MOTORCALIBRATE_MOTORBANGBANG_H
#define MOTORCALIBRATE_MOTORBANGBANG_H

#include "HAL/format/DataObjectEndpoint.h"
#include <boost/function.hpp>
#include <boost/asio.hpp>

namespace subjugator {
	class MotorBangBang {
		public:
			struct Settings {
				double holdtime;
				double maxreference;
				bool random;
			};

			typedef boost::function<void (double)> BangUpdateCallback;

			MotorBangBang(boost::asio::io_service &ioservice, DataObjectEndpoint &endpoint);
			void configureCallbacks(const BangUpdateCallback &bangupdatecallback);

			void start(const Settings &settings);
			void stop();

		private:
			boost::asio::deadline_timer timer;
			DataObjectEndpoint &endpoint;
			Settings settings;

			BangUpdateCallback bangupdatecallback;

			bool toggle;

			void startTimer();
			void timerCallback(const boost::system::error_code &error);
	};
}

#endif
