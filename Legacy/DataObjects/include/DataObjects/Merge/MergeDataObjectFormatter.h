#ifndef DATAOBJECTS_MERGE_MERGEDATAOBJECTFORMATTER_H
#define DATAOBJECTS_MERGE_MERGEDATAOBJECTFORMATTER_H

#include "DataObjects/Embedded/EmbeddedDataObjectFormatter.h"
#include "DataObjects/EmbeddedTypeCodes.h"
#include "DataObjects/EmbeddedAddresses.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class MergeDataObjectFormatter : public EmbeddedDataObjectFormatter {
		public:
			MergeDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress);

		protected:
			virtual DataObject *makeInfoDataObject(ByteVec::const_iterator begin, ByteVec::const_iterator end);
	};
}

#endif

