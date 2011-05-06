#ifndef HAL_SERIALTRANSPORT_H
#define HAL_SERIALTRANSPORT_H

#include "HAL/StreamTransport.h"
#include <vector>
#include <string>

namespace subjugator {
	class SerialTransport : public StreamTransport<boost::asio::serial_port> {
		public:
			SerialTransport(const std::vector<std::string> &devicenames);
			~SerialTransport();
	};
}

#endif
