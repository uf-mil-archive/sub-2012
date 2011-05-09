#ifndef HAL_RAWWIRE_H
#define HAL_RAWWIRE_H

#include "HAL/PacketFormatter.h"
#include "HAL/Transport.h"
#include "HAL/shared.h"
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <string>

namespace subjugator {
	class RawWire {
		public:
			typedef boost::function<PacketFormatter *()> PacketFormatterFactory;
			RawWire(Transport *transport, PacketFormatterFactory packetformatterfactory);

			typedef boost::function<void (int endnum, const Packet &packet)> ReadCallback;
			typedef Transport::ErrorCallback ErrorCallback;
			void configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback);

			void start();
			void stop();

			void writePacket(int endnum, const Packet &packet);

		private:
			boost::scoped_ptr<Transport> transport;
			boost::ptr_vector<PacketFormatter> formatters;

			ReadCallback readcallback;
			ErrorCallback errorcallback;

			void transportReadCallback(int endnum, const ByteVec &bytes);
			void transportErrorCallback(int endnum, const std::string &msg);
	};
}

#endif

