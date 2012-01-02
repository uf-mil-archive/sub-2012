#ifndef HAL_HAL_H
#define HAL_HAL_H

#include "HAL/transport/Transport.h"
#include "HAL/transport/Endpoint.h"
#include "HAL/format/DataObjectEndpoint.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/asio.hpp>

namespace subjugator {
	class BaseHAL {
		public:
			struct EndpointConfiguration {
				std::string protocol;
				std::string protoaddress;
				std::map<std::string, std::string> params;
				
				EndpointConfiguration() { }
				EndpointConfiguration(const std::string &confstr);
			};
		
			BaseHAL();

			void addTransport(Transport *transport);
			void clearTransports();

			Endpoint *makeEndpoint(const EndpointConfiguration &conf);
			DataObjectEndpoint *makeDataObjectEndpoint(const EndpointConfiguration &conf, DataObjectFormatter *dobjformat, PacketFormatter *packetformat);

		private:
			typedef boost::ptr_vector<Transport> TransportVec;
			TransportVec transports;
	};

	class HAL : public BaseHAL {
		public:
			HAL(boost::asio::io_service &io);
	};
}

#endif
