#ifndef DATAOBJECTS_DVLCONFIGURATION_H
#define DATAOBJECTS_DVLCONFIGURATION_H

#include "DVL/DataObjects/DVLCommand.h"

namespace subjugator {
	class DVLConfiguration : public DVLCommand {
		public:
			DVLConfiguration(double maxdepth);

			virtual Packet makePacket() const;

		private:
			double maxdepth;
	};
}

#endif

