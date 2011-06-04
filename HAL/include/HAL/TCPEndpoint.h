#ifndef HAL_TCPENDPOINT_H
#define HAL_TCPENDPOINT_H

#include "HAL/BaseStreamEndpoint.h"

namespace subjugator {
	class TCPEndpoint : public BaseStreamEndpoint<boost::asio::ip::tcp::socket> {
		public:
			TCPEndpoint(const boost::asio::ip::tcp::endpoint &endpoint, IOThread &iothread);

			virtual void open();

		private:
			boost::asio::ip::tcp::endpoint endpoint;

			void connectCallback(const boost::system::error_code& error);

	};
}

#endif

