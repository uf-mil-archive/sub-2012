#ifndef HAL_HAL_H
#define HAL_HAL_H

#include "HAL/AddressTable.h"
#include "HAL/Transport.h"
#include "HAL/Endpoint.h"
#include "HAL/DataObjectEndpoint.h"
#include <boost/ptr_container/ptr_vector.hpp>

namespace subjugator {
	class HAL {
		public:
			HAL();

			void addTransport(Transport *transport);
			void loadAddressFile(const std::string &filename);

			AddressTable &getAddressTable() { return addrtable; }
			const AddressTable &getAddressTable() const { return addrtable; }
			bool addrAvailable(int addr) { return addrtable.hasEntry(addr); }

			Endpoint *openEndpoint(int addr);
			DataObjectEndpoint *openDataObjectEndpoint(int addr, DataObjectFormatter *dobjformat, PacketFormatter *packetformat);

		private:
			AddressTable addrtable;

			typedef boost::ptr_vector<Transport> TransportVec;
			TransportVec transports;
	};
}

#endif
