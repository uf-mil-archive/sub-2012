#ifndef MOTORDRIVERDATAOBJECTFORMATTER_H
#define MOTORDRIVERDATAOBJECTFORMATTER_H

#include "HAL/format/DataObjectFormatter.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class MotorDriverDataObjectFormatter : public DataObjectFormatter {
		public:
			MotorDriverDataObjectFormatter(boost::uint8_t address);

			virtual DataObject *toDataObject(const Packet &packet);

			virtual Packet toPacket(const DataObject &dobj);

		private:
			boost::uint8_t address;

			boost::uint16_t packetcount_out;
			boost::uint16_t packetcount_in;
	};
}

#endif

