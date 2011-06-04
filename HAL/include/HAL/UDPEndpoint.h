#ifndef HAL_UDPENDPOINT_H
#define HAL_UDPENDPOINT_H

#include "HAL/BaseEndpoint.h"
#include "HAL/shared.h"
#include <boost/asio.hpp>

namespace subjugator {
	class UDPEndpoint : public BaseEndpoint {
		public:
			struct TransportCallbacks { // implemented by UDPTransport
				virtual void endpointWrite(UDPEndpoint *endpoint, ByteVec::const_iterator begin, ByteVec::const_iterator end) = 0;
				virtual void endpointDeleted(UDPEndpoint *endpoint) = 0;
			};

			UDPEndpoint(const boost::asio::ip::udp::endpoint &endpoint, IOThread &iothread, TransportCallbacks &callbacks);
			virtual ~UDPEndpoint();

			virtual void open();
			virtual void close();
			virtual void write(ByteVec::const_iterator begin, ByteVec::const_iterator end);

			inline const boost::asio::ip::udp::endpoint &getEndpoint() const { return endpoint; }
			void packetReceived(ByteVec::const_iterator begin, ByteVec::const_iterator end);

		private:
			boost::asio::ip::udp::endpoint endpoint;
			TransportCallbacks &callbacks;
	};
}

#endif

