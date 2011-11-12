#ifndef DATAOBJECTS_MOTORDRIVER_MOTORDRIVERDATAOBJECTFORMATTER_H
#define DATAOBJECTS_MOTORDRIVER_MOTORDRIVERDATAOBJECTFORMATTER_H

#include "LibSub/DataObjects/EmbeddedDataObjectFormatter.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class MotorDriverDataObjectFormatter : public EmbeddedDataObjectFormatter {
		public:
			MotorDriverDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode=BRUSHEDOPEN);

		protected:
			virtual DataObject *makeInfoDataObject(ByteVec::const_iterator begin, ByteVec::const_iterator end);
	};
}

#endif

