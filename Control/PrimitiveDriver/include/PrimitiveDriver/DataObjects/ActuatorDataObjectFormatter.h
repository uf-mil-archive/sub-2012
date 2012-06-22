#ifndef PRIMITIVEDRIVER_DATAOBJECTS_ACTUATORDATAOBJECTFORMATTER_H
#define PRIMITIVEDRIVER_DATAOBJECTS_ACTUATORDATAOBJECTFORMATTER_H

#include <HAL/format/DataObjectFormatter.h>

namespace subjugator {
	class ActuatorDataObjectFormatter : public DataObjectFormatter {
		virtual ~ActuatorDataObjectFormatter() { }

		virtual DataObject *toDataObject(const Packet &packet);
		virtual Packet toPacket(const DataObject &dobj);
	};
}

#endif
