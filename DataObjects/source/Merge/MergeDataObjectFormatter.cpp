#include "DataObjects/Merge/MergeDataObjectFormatter.h"
#include "DataObjects/Merge/MergeInfo.h"

using namespace subjugator;
using namespace boost;

MergeDataObjectFormatter::MergeDataObjectFormatter(uint8_t devaddress, uint8_t pcaddress)
: EmbeddedDataObjectFormatter(devaddress, pcaddress, MERGEBOARD) { }

DataObject *MergeDataObjectFormatter::makeInfoDataObject(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	return new MergeInfo(begin, end);
}

