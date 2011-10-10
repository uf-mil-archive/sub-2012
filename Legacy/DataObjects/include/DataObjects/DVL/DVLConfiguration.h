#ifndef DATAOBJECTS_DVLCONFIGURATION_H
#define DATAOBJECTS_DVLCONFIGURATION_H

#include "DataObjects/DVL/DVLCommand.h"

namespace subjugator {
	class DVLConfiguration : public DVLCommand {
		public:
			DVLConfiguration(double maxdepth, double alignmentdeg);

			virtual Packet makePacket() const;

		private:
			double maxdepth;
			double alignmentdeg;
	};
}

#endif

