#ifndef DATAOBJECTS_EMBEDDED_STOPPUBLISHING_H
#define DATAOBJECTS_EMBEDDED_STOPPUBLISHING_H

#include "LibSub/DataObjects/EmbeddedCommand.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class StopPublishing : public EmbeddedCommand {
		public:
			StopPublishing();

			virtual uint8_t getToken() const { return 2; }
	};
}

#endif

