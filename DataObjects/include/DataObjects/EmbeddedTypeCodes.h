#ifndef HAL_EMBEDDEDTYPECODES_H
#define HAL_EMBEDDEDTYPECODES_H

namespace subjugator {
	enum EmbeddedTypeCode {
		BRUSHEDOPEN = 0,
		BRUSHEDCLOSED = 1,
		BRUSHLESSOPEN = 2,
		BRUSHLESSCLOSED = 3,
		DEPTH = 4,
		MERGEBOARD = 6,

		HEARTBEAT = 100,
		ESTOP = 101
	};
}

#endif

