#include "PrimitiveDriver/DataObjects/MotorDriverDataObjectFormatter.h"
#include "PrimitiveDriver/DataObjects/MotorDriverInfo.h"

using namespace subjugator;
using namespace boost;

MotorDriverDataObjectFormatter::MotorDriverDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode)
: EmbeddedDataObjectFormatter(devaddress, pcaddress, typecode) { }

DataObject *MotorDriverDataObjectFormatter::makeInfoDataObject(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	return new MotorDriverInfo(begin, end);
}

