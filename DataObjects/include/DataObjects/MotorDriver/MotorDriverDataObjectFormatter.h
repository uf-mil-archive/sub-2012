#ifndef DATAOBJECTS_MOTORDRIVER_MOTORDRIVERDATAOBJECTFORMATTER_H
#define DATAOBJECTS_MOTORDRIVER_MOTORDRIVERDATAOBJECTFORMATTER_H

#include "DataObjects/EmbeddedTypeCodes.h"
#include "HAL/format/DataObjectFormatter.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class MotorDriverDataObjectFormatter : public DataObjectFormatter {
		public:
			MotorDriverDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode);

			virtual DataObject *toDataObject(const Packet &packet);

			virtual Packet toPacket(const DataObject &dobj);

		private:
			boost::uint8_t devaddress;
			boost::uint8_t pcaddress;
			EmbeddedTypeCode typecode;

			boost::uint16_t packetcount_out;
			boost::uint16_t packetcount_in;
	};
}

#endif

