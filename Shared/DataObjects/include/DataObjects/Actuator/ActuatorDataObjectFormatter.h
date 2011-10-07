#ifndef DATAOBJECTS_MERGE_ACTUATORDATAOBJECTFORMATTER_H
#define DATAOBJECTS_MERGE_ACTUATORDATAOBJECTFORMATTER_H

#include "HAL/format/DataObjectFormatter.h"

namespace subjugator {
	class ActuatorDataObjectFormatter : public DataObjectFormatter {
		public:
			ActuatorDataObjectFormatter();

			virtual DataObject *toDataObject(const Packet &packet);
			virtual Packet toPacket(const DataObject &dobj);
	};
}

#endif

