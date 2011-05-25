#ifndef HAL_DATAOBJECTFORMATTER_H
#define HAL_DATAOBJECTFORMATTER_H

#include "HAL/DataObject.h"
#include "HAL/shared.h"

namespace subjugator {
	class DataObjectFormatter {
		public:
			virtual ~DataObjectFormatter() { }
			virtual DataObject *toDataObject(const ByteVec &bytes) =0;
	};
}

#endif
