#ifndef HAL_SERIALENDPOINT_H
#define HAL_SERIALENDPOINT_H

#include "HAL/transport/BaseStreamEndpoint.h"

namespace subjugator {
	class SerialEndpoint : public BaseStreamEndpoint<boost::asio::serial_port> {
		public:
			SerialEndpoint(const std::string &devicename, int baud, boost::asio::io_service &io);

			virtual void open();

		private:
			std::string devicename;
			int baud;
	};
}

#endif

