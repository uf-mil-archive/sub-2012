#ifndef HAL_HAL_H
#define HAL_HAL_H

#include "HAL/shared.h"
#include "HAL/Transport.h"
#include "HAL/RawWire.h"
#include "HAL/DataObjectFormatter.h"
#include <boost/scoped_ptr.hpp>
#include <memory>

namespace subjugator {
	class HAL {
		public:
			HAL(DataObjectFormatter *dataobjectformat, Transport *transport, RawWire::PacketFormatterFactory packetformatfactory);

			typedef boost::function<void (int endnum, std::auto_ptr<DataObject> &dobj)> ReadCallback;

			typedef Transport::ErrorCallback ErrorCallback;

			void configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback);

			void write(int endnum, const DataObject &dataobject);

		private:
			boost::scoped_ptr<DataObjectFormatter> dataobjectformat;
			RawWire rawwire;

			ReadCallback readcallback;
			ErrorCallback errorcallback;

			void transportReadCallback(int endnum, const ByteVec &bytes);
			void transportErrorCallback(int endnum, const std::string &mgs);
	};
}

#endif

