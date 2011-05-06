#ifndef HAL_TCPTRANSPORT_H
#define HAL_TCPTRANSPORT_H

#include "HAL/shared.h"
#include "HAL/Transport.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/cstdint.hpp>
#include <utility>
#include <vector>
#include <queue>

namespace subjugator {
	class TCPTransport : public Transport {
		public:
			typedef std::pair<std::string, int> EndpointConfig;
			TCPTransport(const std::vector<EndpointConfig> &endpointconfigs);
			~TCPTransport();

			virtual int getEndpointCount() const;
			virtual void configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback);
			virtual void write(int endnum, const ByteVec &bytes);

		private:
			boost::asio::io_service ioservice;
			boost::thread iothread;

			struct EndpointData {
				EndpointData(boost::asio::io_service &ioservice);

				boost::asio::ip::tcp::socket socket;
				ByteVec sendbuf;
				ByteVec recvbuf;
			};

			boost::ptr_vector<EndpointData> endpointdatavec;

			ReadCallback readcallback;
			ErrorCallback errorcallback;

			void asioAppendSendBufCallback(int endnum, const ByteVec &bytes);
			void asioReceiveCallback(int endnum, const boost::system::error_code& error, std::size_t bytes);
			void asioSendCallback(int endnum, const boost::system::error_code& error, std::size_t bytes);
			void asioConnectCallback(int endnum, const boost::system::error_code& error);

			void startAsyncReceive(int endnum);
			void startAsyncSend(int endnum);
	};
}

#endif
