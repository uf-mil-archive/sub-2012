#ifndef HAL_UDPTRANSPORT_H
#define HAL_UDPTRANSPORT_H

#include "HAL/shared.h"
#include "HAL/transport/Transport.h"
#include "HAL/transport/UDPEndpoint.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <utility>
#include <vector>
#include <queue>

namespace subjugator {
	class UDPTransport : public Transport, UDPEndpoint::TransportCallbacks {
		public:
			UDPTransport(boost::asio::io_service &io, int port=50000);

			virtual const std::string &getName() const;
			virtual Endpoint *makeEndpoint(const std::string &address, const ParamMap &params);

		private:
			int port;

			typedef std::vector<UDPEndpoint *> EndpointPtrVec;
			EndpointPtrVec endpoints;
			boost::asio::ip::udp::socket socket;

			ByteVec recvbuffer;
			boost::asio::ip::udp::endpoint recvendpoint;

			std::queue<std::pair<boost::asio::ip::udp::endpoint, ByteVec> > sendqueue;

			virtual void endpointOpened(UDPEndpoint *endpoint);
			virtual void endpointWrite(UDPEndpoint *endpoint, ByteVec::const_iterator begin, ByteVec::const_iterator end);
			virtual void endpointClosed(UDPEndpoint *endpoint);
			virtual void endpointDeleted(UDPEndpoint *endpoint);
			virtual const std::string &getEndpointError() const;

			void pushSendQueueCallback(const boost::asio::ip::udp::endpoint &endpoint, const ByteVec &bytes);
			void receiveCallback(const boost::system::error_code& error, std::size_t bytes);
			void sendCallback(const boost::system::error_code& error, std::size_t bytes);

			void startAsyncReceive();
			void startAsyncSend();

			void setError(const std::string &errmsg);
			std::string errmsg;
	};
}

#endif
