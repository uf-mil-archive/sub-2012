#ifndef DATAOBJECTS_MERGE_MERGEACTUATOR_H
#define DATAOBJECTS_MERGE_MERGEACTUATOR_H

#include "DataObjects/Embedded/EmbeddedCommand.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class MergeActuator : public EmbeddedCommand {
		public:
			MergeActuator() : flags(0) { }
			MergeActuator(int flags) : flags(flags) { }
			
			int getFlags() const { return flags; }
			
			virtual uint8_t getToken() const { return 6; }
			virtual void appendDataPacket(Packet &packet) const { packet.push_back((boost::uint8_t)flags); }
		
		private:
			int flags;
	};
}
	
#endif

