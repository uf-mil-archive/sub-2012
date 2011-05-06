#ifndef HAL_TCPTRANSPORT_H
#define HAL_TCPTRANSPORT_H

#include "HAL/StreamTransport.h"
#include <utility>
#include <vector>
#include <string>

namespace subjugator {
	class TCPTransport : public StreamTransport<boost::asio::ip::tcp::socket> {
		public:
			typedef std::pair<std::string, int> EndpointConfig;
			TCPTransport(const std::vector<EndpointConfig> &endpointconfigs);
			~TCPTransport();

		private:
			void asioConnectCallback(int endnum, const boost::system::error_code& error);
	};
}

#endif
