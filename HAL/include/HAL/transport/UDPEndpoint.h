#ifndef HAL_UDPENDPOINT_H
#define HAL_UDPENDPOINT_H

#include "HAL/transport/BaseEndpoint.h"
#include "HAL/shared.h"
#include <boost/asio.hpp>

namespace subjugator {
	class UDPEndpoint : public Endpoint {
		public:
			struct TransportCallbacks { // implemented by UDPTransport
				virtual void endpointOpened(UDPEndpoint *endpoint) = 0;
				virtual void endpointWrite(UDPEndpoint *endpoint, ByteVec::const_iterator begin, ByteVec::const_iterator end) = 0;
				virtual void endpointClosed(UDPEndpoint *endpoint) = 0;
				virtual void endpointDeleted(UDPEndpoint *endpoint) = 0;
				virtual const std::string &getEndpointError() const = 0;
			};

			UDPEndpoint(const boost::asio::ip::udp::endpoint &endpoint, TransportCallbacks &callbacks);
			virtual ~UDPEndpoint();

			virtual void configureCallbacks(const ReadCallback &readcallback, const StateChangeCallback &statechangecallback);
			virtual State getState() const;
			virtual const std::string &getErrorMessage() const;

			virtual void open();
			virtual void close();
			virtual void write(ByteVec::const_iterator begin, ByteVec::const_iterator end);

			inline const boost::asio::ip::udp::endpoint &getEndpoint() const { return endpoint; }
			void packetReceived(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			void errorChanged();

		private:
			boost::asio::ip::udp::endpoint endpoint;
			TransportCallbacks &callbacks;
			bool opened;

			ReadCallback readcallback;
			StateChangeCallback statechangecallback;

			void callStateChangeCallback();
	};
}

#endif

