#ifndef HAL_SERIALTRANSPORT_H
#define HAL_SERIALTRANSPORT_H

#include "HAL/transport/Transport.h"
#include <boost/asio.hpp>
#include <utility>
#include <vector>
#include <string>

namespace subjugator {
	class SerialTransport : public Transport {
		public:
			SerialTransport(boost::asio::io_service &io);

			virtual const std::string &getName() const;
			virtual Endpoint *makeEndpoint(const std::string &address, const ParamMap &params);

		private:
			boost::asio::io_service &io;
	};
}

#endif
