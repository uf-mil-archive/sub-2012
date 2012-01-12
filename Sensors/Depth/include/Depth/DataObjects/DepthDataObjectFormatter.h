#ifndef DEPTH_DATAOBJECTS_DEPTHDATAOBJECTFORMATTER_H
#define DEPTH_DATAOBJECTS_DEPTHDATAOBJECTFORMATTER_H

#include "LibSub/DataObjects/EmbeddedDataObjectFormatter.h"
#include "LibSub/DataObjects/EmbeddedTypeCodes.h"
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

