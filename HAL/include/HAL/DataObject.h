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

			/** \brief Serializes a DataObject

			Converts a DataObject to a Packet (or ByteVec), for transmission
			using the HAL.
			*/
			virtual Packet toPacket() const =0;
	};
}

#endif

