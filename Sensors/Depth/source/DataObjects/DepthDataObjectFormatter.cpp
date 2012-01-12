#include "Depth/DataObjects/DepthDataObjectFormatter.h"
#include "Depth/DataObjects/DepthInfo.h"

using namespace subjugator;
using namespace boost;

DepthDataObjectFormatter::DepthDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode)
: EmbeddedDataObjectFormatter(devaddress, pcaddress, typecode) { }

DataObject *DepthDataObjectFormatter::makeInfoDataObject(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	return new DepthInfo(begin, end);
}
