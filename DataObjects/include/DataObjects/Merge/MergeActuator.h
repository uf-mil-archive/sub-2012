#ifndef DATAOBJECTS_MERGE_MERGEACTUATOR_H
#define DATAOBJECTS_MERGE_MERGEACTUATOR_H

#include "DataObjects/Embedded/EmbeddedCommand.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class MergeActuator : public EmbeddedCommand {
		public:
			MergeActuator() : flags(0) { }
			MergeActuator(int flags) : flags(flags) { }

			static const int GRABBER_LEFT = (1 << 4);
			static const int GRABBER_RIGHT = (1 << 3);
			static const int SHOOTER_LEFT = (1 << 2);
			static const int SHOOTER_RIGHT = (1 << 1);
			static const int BALL_DROPPER = (1 << 0);

			int getFlags() const { return flags; }

			virtual uint8_t getToken() const { return 3; }
			virtual void appendDataPacket(Packet &packet) const { packet.push_back((boost::uint8_t)flags); }

		private:
			int flags;
	};
}

#endif

