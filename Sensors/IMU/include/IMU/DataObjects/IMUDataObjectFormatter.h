#ifndef IMU_DATA_OBJECT_FORMATTER_H
#define IMU_DATA_OBJECT_FORMATTER_H

#include "LibSub/DataObjects/EmbeddedDataObjectFormatter.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class IMUDataObjectFormatter : public DataObjectFormatter {
		public:
			IMUDataObjectFormatter();

			virtual DataObject *toDataObject(const Packet &packet);
			virtual Packet toPacket(const DataObject &dobj);
	};
}

#endif	// IMU_DATA_OBJECT_FORMATTER_H

