/**
\class subjugator::RawWire
\brief HAL class providing packet level communication
\headerfile HAL/RawWire.h

RawWire enables the HAL to exchange packets with the hardware, using
a PacketFormatter to add and remove headers, footers, and checksums to
the data before sending it over the correct physical medium using a Transport.
A single RawWire has multiple endpoints numbered from zero, which represent multiple
destination devices. Endpoints are configured using the constructor of the Transport class.
*/

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
			/** \brief Type of function used to create Packet Formatters */

			typedef boost::function<PacketFormatter *()> PacketFormatterFactory;

			/** \brief Initializes a RawWire
			Constructs a RawWire, using a heap-allocated Transport, and a PacketFormatterFactory.
			RawWire takes ownership of the Transport, and will free it when RawWire is destructed.
			The constructor requires a PacketFormatterFactory, because it needs to create a packet formatter
			for every endpoint in the Transport.
			*/

			RawWire(Transport *transport, PacketFormatterFactory packetformatterfactory);

			typedef boost::function<void (int endnum, const Packet &packet)> ReadCallback; //< Type of function required for a read callback. Called when a valid packet is found on an endpoint
			typedef Transport::ErrorCallback ErrorCallback; //< Type of function required for an error callback.

			/** \brief Set functions to be used as callbacks

			configureCallbacks() is called by the HAL early on to provide the RawWire with HAL's desired callback functions.
			It must be called before start(), or else the user could miss incoming data or errors. */

			void configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback);

			/** \brief Starts or resumes a RawWire

			start() causes the RawWire to start the underlying Transport.
			*/

			void start();

			/** \brief Stops a RawWire

			stop() causes the RawWire to stop the underlying Transport.
			*/

			void stop();

			/** \brief Writes a packet

			writePacket() writes a packet to the specified endpoint using the
			the correct PacketFormatter and underlying Transport.
			*/

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

