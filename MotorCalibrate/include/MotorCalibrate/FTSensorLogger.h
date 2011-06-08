#ifndef MOTORCALIBRATE_FTSENSORLOGGER_H
#define MOTORCALIBRATE_FTSENSORLOGGER_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <ostream>
#include <sstream>

namespace subjugator {
	class FTSensorLogger {
		public:
			struct LogEntry {
				int fx, fy, fz;
				int mx, my, mz;
			};

			typedef boost::function<void (const LogEntry &)> LogCallback;

			FTSensorLogger(const std::string &device, boost::asio::io_service &ioservice, const LogCallback &logcallback);

			void bias();

			void begin();
			void end();

		private:
			bool running;

			std::string device;
			boost::asio::serial_port port;
			boost::asio::streambuf recvbuf;
			std::istream recvstream;

			LogCallback logcallback;

			void receiveCallback(const boost::system::error_code& error, std::size_t bytes);

			void sendCommand(const std::string &command);
			void sendCommandACK(const std::string &command);

			static void getlineCRLN(std::istream &in, std::string &out);
			void sync();
			void clearReadBuffer();
	};
}

#endif

