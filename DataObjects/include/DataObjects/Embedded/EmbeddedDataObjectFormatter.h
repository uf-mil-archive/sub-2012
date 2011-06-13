#ifndef DATAOBJECTS_EMBEDDED_EMBEDDEDDATAOBJECTFORMATTER_H
#define DATAOBJECTS_EMBEDDED_EMBEDDEDDATAOBJECTFORMATTER_H

#include "DataObjects/EmbeddedTypeCodes.h"
#include "HAL/format/DataObjectFormatter.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class EmbeddedDataObjectFormatter : public DataObjectFormatter {
		public:
			EmbeddedDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode);

			virtual DataObject *toDataObject(const Packet &packet);
			virtual Packet toPacket(const DataObject &dobj);

		protected:
			virtual DataObject *makeInfoDataObject(ByteVec::const_iterator begin, ByteVec::const_iterator end) =0;

		private:
			boost::uint8_t devaddress;
			boost::uint8_t pcaddress;
			EmbeddedTypeCode typecode;

			boost::uint16_t packetcount_out;
			boost::uint16_t packetcount_in;
	};
}

#endif

