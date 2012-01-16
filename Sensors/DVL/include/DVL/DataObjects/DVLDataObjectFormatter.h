#ifndef DATAOBJECTS_DVL_DVLDATAOBJECTFORMATTER_H
#define DATAOBJECTS_DVL_DVLDATAOBJECTFORMATTER_H

#include "HAL/format/DataObjectFormatter.h"

namespace subjugator {
	class DVLDataObjectFormatter : public DataObjectFormatter {
		public:
			DVLDataObjectFormatter();

			virtual DataObject *toDataObject(const Packet &packet);
			virtual Packet toPacket(const DataObject &dobj);
	};
}

#endif

