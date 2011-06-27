#ifndef DATAOBJECTS_HYDROPHONE_HYDROPHONEDATAOBJECTFORMATTER_H
#define DATAOBJECTS_HYDROPHONE_HYDROPHONEDATAOBJECTFORMATTER_H

#include "HAL/format/DataObjectFormatter.h"

namespace subjugator {
	class HydrophoneDataObjectFormatter : public DataObjectFormatter {
		public:
			HydrophoneDataObjectFormatter();

			virtual DataObject *toDataObject(const Packet &packet);
			virtual Packet toPacket(const DataObject &dobj);
	};
}

#endif


