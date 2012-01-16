#ifndef DATAOBJECTS_DVLBREAK_H
#define DATAOBJECTS_DVLBREAK_H

#include "DVL/DataObjects/DVLCommand.h"

namespace subjugator {
	class DVLBreak : public DVLCommand {
		public:
			DVLBreak();

			virtual Packet makePacket() const;
	};
}

#endif

