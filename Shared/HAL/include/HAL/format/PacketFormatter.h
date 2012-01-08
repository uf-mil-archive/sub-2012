/**
\class subjugator::PacketFormatter
\headerfile HAL/RawWire.h
\brief HAL class for formatting packets to be transmitted on a Transport

The PacketFormatter interface converts between Packets and ByteVecs according to
a specific packet format, usually adding some kind of header or footer
bytes. If the PacketFormat is checksum-independent, it may accept a checksum
function in its constructor.
*/

#ifndef HAL_PACKETFORMATTER_H
#define HAL_PACKETFORMATTER_H

#include "HAL/shared.h"
#include <vector>

namespace subjugator {
	class PacketFormatter {
		public:
			virtual ~PacketFormatter() { }

			/** \brief Extract packets from incoming data.

			Returns a vector containing zero or more packets successfully extracted from the data.
			It is up to the PacketFormatter to buffer data internally that may represent an incomplete packet. */
			virtual std::vector<Packet> parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end) = 0;

			/** \brief Format a packet for transmission.

			Formats a packet for transmission, generating checksums and adding headers and footers according to
			the format. */
			virtual ByteVec formatPacket(const Packet &packet) const = 0;
	};
}

#endif

