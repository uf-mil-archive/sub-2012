#ifndef HAL_HAL_H
#define HAL_HAL_H

#include "HAL/transport/Transport.h"
#include "HAL/transport/Endpoint.h"
#include "HAL/format/DataObjectEndpoint.h"
#include <boost/ptr_container/ptr_vector.hpp>

namespace subjugator {
	class HAL {
		public:
			struct EndpointConfiguration {
				std::string protocol;
				std::string protoaddress;
				std::map<std::string, std::string> params;
				
				EndpointConfiguration() { }
				EndpointConfiguration(const std::string &confstr);
			};
		
			HAL();

			void addTransport(Transport *transport);
			void clearTransports();

			Endpoint *openEndpoint(const EndpointConfiguration &conf);
			DataObjectEndpoint *openDataObjectEndpoint(const EndpointConfiguration &conf, DataObjectFormatter *dobjformat, PacketFormatter *packetformat);

		private:
			typedef boost::ptr_vector<Transport> TransportVec;
			TransportVec transports;
	};
}

#endif
