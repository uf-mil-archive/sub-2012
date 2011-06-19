#include "DataObjects/DepthBoard/DepthBoardDataObjectFormatter.h"
#include "DataObjects/DepthBoard/DepthBoardInfo.h"

using namespace subjugator;
using namespace boost;

DepthBoardDataObjectFormatter::DepthBoardDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode)
: EmbeddedDataObjectFormatter(devaddress, pcaddress, typecode) { }

DataObject *DepthBoardDataObjectFormatter::makeInfoDataObject(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	return new DepthBoardInfo(begin, end);
}

