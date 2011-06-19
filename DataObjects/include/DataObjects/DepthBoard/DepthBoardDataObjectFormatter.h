#ifndef DATAOBJECTS_DEPTHBOARD_DEPTHBOARDDATAOBJECTFORMATTER_H
#define DATAOBJECTS_DEPTHBOARD_DEPTHBOARDDATAOBJECTFORMATTER_H

#include "DataObjects/Embedded/EmbeddedDataObjectFormatter.h"
#include "DataObjects/EmbeddedTypeCodes.h"
#include "DataObjects/EmbeddedAddresses.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class DepthBoardDataObjectFormatter : public EmbeddedDataObjectFormatter {
		public:
			DepthBoardDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode=DEPTHBOARD);

		protected:
			virtual DataObject *makeInfoDataObject(ByteVec::const_iterator begin, ByteVec::const_iterator end);
	};
}

#endif

