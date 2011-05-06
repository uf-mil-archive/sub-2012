#ifndef HAL_SERIALTRANSPORT_H
#define HAL_SERIALTRANSPORT_H

#include "HAL/shared.h"
#include "HAL/Transport.h"
#include "HAL/TransportBase.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/cstdint.hpp>
#include <utility>
#include <vector>
#include <queue>

namespace subjugator {
	class SerialTransport : public ASIOTransportBase {
		public:
			SerialTransport(const std::vector<std::string> &devicenames);
			~SerialTransport();

			virtual int getEndpointCount() const;
			virtual void write(int endnum, const ByteVec &bytes);

		private:
			struct EndpointData {
				EndpointData(boost::asio::io_service &ioservice);

				boost::asio::serial_port port;
				ByteVec sendbuf;
				ByteVec recvbuf;
			};

			boost::ptr_vector<EndpointData> endpointdatavec;

			void asioAppendSendBufCallback(int endnum, const ByteVec &bytes);
			void asioReceiveCallback(int endnum, const boost::system::error_code& error, std::size_t bytes);
			void asioSendCallback(int endnum, const boost::system::error_code& error, std::size_t bytes);
			void asioConnectCallback(int endnum);

			void startAsyncReceive(int endnum);
			void startAsyncSend(int endnum);
	};
}

#endif
