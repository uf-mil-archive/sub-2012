#ifndef HAL_DATAOBJECTENDPOINT_H
#define HAL_DATAOBJECTENDPOINT_H

#include "HAL/transport/Endpoint.h"
#include "HAL/format/DataObjectFormatter.h"
#include "HAL/format/PacketFormatter.h"
#include <boost/scoped_ptr.hpp>
#include <memory>

namespace subjugator {
	class DataObjectEndpoint {
		public:
			DataObjectEndpoint(Endpoint *endpoint, DataObjectFormatter *dobjformat, PacketFormatter *packetformat);

			typedef boost::function<void (std::auto_ptr<DataObject> &dobj)> ReadCallback;
			typedef boost::function<void ()> StateChangeCallback;

			void configureCallbacks(const ReadCallback &readcallback, const StateChangeCallback &statechangecallback);

			void write(const DataObject &dobj);

		private:
			boost::scoped_ptr<Endpoint> endpoint;
			boost::scoped_ptr<DataObjectFormatter> dobjformat;
			boost::scoped_ptr<PacketFormatter> packetformat;

			ReadCallback readcallback;
			StateChangeCallback statechangecallback;

			void endpointReadCallback(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			void endpointStateChangedCallback();
	};
}

#endif

