#ifndef HAL_TCPTRANSPORT_H
#define HAL_TCPTRANSPORT_H

#include "HAL/transport/Transport.h"
#include <boost/asio.hpp>
#include <utility>
#include <vector>
#include <string>

namespace subjugator {
	class TCPTransport : public Transport {
		public:
			TCPTransport(boost::asio::io_service &ioservice);

			virtual const std::string &getName() const;
			virtual Endpoint *makeEndpoint(const std::string &address, const ParamMap &params);

		private:
			boost::asio::io_service &ioservice;
	};
}

#endif
