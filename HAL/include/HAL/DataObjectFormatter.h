/**
\class subjugator::DataObjectFormatter
\headerfile HAL/DataObjectFormatter.h
\brief Interface used by HAL to convert incoming bytes to DataObjects

A DataObjectFormatter is in charge of looking at the contents of a Packet generated
by a PacketFormatter, and creating an appropriate DataObject from its contents.
*/

#ifndef HAL_DATAOBJECTFORMATTER_H
#define HAL_DATAOBJECTFORMATTER_H

#include "HAL/DataObject.h"
#include "HAL/shared.h"

namespace subjugator {
	class DataObjectFormatter {
		public:
			virtual ~DataObjectFormatter() { }

			/**
			\brief Create a DataObject from a Packet

			toDataObject instantiates the correct DataObject on the heap from the contents of the Packet, and returns it.
			*/
			virtual DataObject *toDataObject(const Packet &packet) =0;
	};
}

#endif
