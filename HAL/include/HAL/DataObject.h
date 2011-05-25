#ifndef HAL_DATAOBJECT_H
#define HAL_DATAOBJECT_H

#include "HAL/shared.h"

namespace subjugator {
	class DataObject {
		public:
			virtual ~DataObject() { }
			virtual Packet toPacket() const =0;
	};
}

#endif

