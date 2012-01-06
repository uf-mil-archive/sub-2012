/**
\class subjugator::DataObject
\headerfile HAL/DataObject.h

\brief Interface representing an object to be sent or received using the HAL.
*/

#ifndef HAL_DATAOBJECT_H
#define HAL_DATAOBJECT_H

#include "HAL/shared.h"

namespace subjugator {
	class DataObject {
		public:
			virtual ~DataObject() { }
	};
}

#endif

