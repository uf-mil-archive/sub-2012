#ifndef MOTORCALIBRATE_MOTORRAMPER_H
#define MOTORCALIBRATE_MOTORRAMPER_H

#include "HAL/format/DataObjectEndpoint.h"
#include <boost/function.hpp>
#include <boost/asio.hpp>

namespace subjugator {
	class MotorRamper {
		public:
			enum Direction {
				Both,
				Forward,
				Reverse
			};

			struct Settings {
				double holdtime;
				double ramptime;
				int divisions;
				double maxreference;
				bool repeat;
				Direction direction;
			};

			typedef boost::function<void (double)> RampUpdateCallback;
			typedef boost::function<void ()> RampCompleteCallback;

			MotorRamper(boost::asio::io_service &ioservice, DataObjectEndpoint &endpoint);
			void configureCallbacks(const RampUpdateCallback &rampupdatecallback, const RampCompleteCallback &rampcompletecallback);

			void start(const Settings &settings);
			void stop();

		private:
			static const double updateRateHz = 10;

			boost::asio::deadline_timer timer;
			DataObjectEndpoint &endpoint;
			Settings settings;

			RampUpdateCallback rampupdatecallback;
			RampCompleteCallback rampcompletecallback;

			bool rampdown;
			int curdivision;
			int counter;

			Direction direction;

			void startTimer();
			void timerCallback(boost::system::error_code &error);
	};
}

#endif
