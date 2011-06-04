#ifndef HAL_SUB7EPACKETFORMATTER_H
#define HAL_SUB7EPACKETFORMATTER_H

#include "HAL/format/ByteDelimitedPacketFormatter.h"
#include "HAL/format/CRCChecksum.h"

namespace subjugator {
	class Sub7EPacketFormatter : public ByteDelimitedPacketFormatter {
		public:
			enum {
				FlagByte = 0x7E,
				EscapeByte = 0x7D,
				EscapeMask = 0x20
			};

			Sub7EPacketFormatter() : ByteDelimitedPacketFormatter(FlagByte, EscapeByte, EscapeMask, new CRCChecksum()) { }
	};
}

#endif

