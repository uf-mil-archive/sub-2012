#ifndef DATAOBJECTS_DEPTH_DEPTHDATAOBJECTFORMATTER_H
#define DATAOBJECTS_DEPTH_DEPTHDATAOBJECTFORMATTER_H

#include "DataObjects/Embedded/EmbeddedDataObjectFormatter.h"
#include "DataObjects/EmbeddedTypeCodes.h"
#include "DataObjects/EmbeddedAddresses.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class DepthDataObjectFormatter : public EmbeddedDataObjectFormatter {
		public:
			DepthDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode=DEPTH);

		protected:
			virtual DataObject *makeInfoDataObject(ByteVec::const_iterator begin, ByteVec::const_iterator end);
	};
}

#endif

