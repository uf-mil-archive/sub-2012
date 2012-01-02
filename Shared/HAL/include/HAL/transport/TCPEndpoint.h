#ifndef HAL_TCPENDPOINT_H
#define HAL_TCPENDPOINT_H

#include "HAL/transport/BaseStreamEndpoint.h"

namespace subjugator {
	class TCPEndpoint : public BaseStreamEndpoint<boost::asio::ip::tcp::socket> {
		public:
			TCPEndpoint(const boost::asio::ip::tcp::endpoint &endpoint, boost::asio::io_service &io);

			virtual void open();

		private:
			boost::asio::ip::tcp::endpoint endpoint;

			void connectCallback(const boost::system::error_code& error);

	};
}

#endif

