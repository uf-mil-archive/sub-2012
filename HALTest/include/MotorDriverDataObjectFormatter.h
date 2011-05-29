#ifndef MOTORDRIVERDATAOBJECTFORMATTER_H
#define MOTORDRIVERDATAOBJECTFORMATTER_H

#include "HAL/DataObjectFormatter.h"

namespace subjugator {
	class MotorDriverDataObjectFormatter : public DataObjectFormatter {
		public:
			MotorDriverDataObjectFormatter();

			virtual DataObject *toDataObject(const Packet &packet);

			virtual Packet toPacket(const DataObject &dobj);

		private:
			uint16_t packetcount_out;
			uint16_t packetcount_in;
	};
}

#endif

