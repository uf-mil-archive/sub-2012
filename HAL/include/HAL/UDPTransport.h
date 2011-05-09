#ifndef HAL_UDPTRANSPORT_H
#define HAL_UDPTRANSPORT_H

#include "HAL/shared.h"
#include "HAL/Transport.h"
#include "HAL/TransportBase.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <utility>
#include <vector>
#include <queue>

namespace subjugator {
	class UDPTransport : public ASIOTransportBase {
		public:
			typedef std::pair<std::string, int> EndpointConfig;
			UDPTransport(const std::vector<EndpointConfig> &endpointconfigs);
			~UDPTransport();

			virtual void start();
			virtual void stop();

			virtual int getEndpointCount() const;
			virtual void write(int endnum, const ByteVec &bytes);

		private:
			boost::asio::io_service ioservice;
			boost::thread iothread;

			boost::asio::ip::udp::socket socket;
			std::vector<boost::asio::ip::udp::endpoint> endpoints;

			ByteVec recvbuffer;
			boost::asio::ip::udp::endpoint recvendpoint;

			std::queue<std::pair<int, ByteVec> > sendqueue;

			void asioPushSendQueueCallback(int endnum, const ByteVec &bytes);
			void asioReceiveCallback(const boost::system::error_code& error, std::size_t bytes);
			void asioSendCallback(const boost::system::error_code& error, std::size_t bytes);

			void startAsyncReceive();
			void startAsyncSend();
	};
}

#endif
