#ifndef HAL_SERIALENDPOINT_H
#define HAL_SERIALENDPOINT_H

#include "HAL/BaseStreamEndpoint.h"

namespace subjugator {
	class SerialEndpoint : public BaseStreamEndpoint<boost::asio::serial_port> {
		public:
			SerialEndpoint(const std::string &devicename, int baud, IOThread &iothread);

			virtual void open();

		private:
			std::string devicename;
			int baud;
	};
}

#endif

