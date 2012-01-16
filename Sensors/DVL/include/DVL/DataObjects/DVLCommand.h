#ifndef DATAOBJECTS_DVL_DVLCOMMAND_H
#define DATAOBJECTS_DVL_DVLCOMMAND_H

#include "HAL/format/DataObject.h"

namespace subjugator {
	class DVLCommand : public DataObject {
		public:
			virtual Packet makePacket() const = 0;
	};
}

#endif

