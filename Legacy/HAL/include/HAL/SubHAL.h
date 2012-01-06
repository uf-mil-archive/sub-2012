#ifndef HAL_SUBHAL_H
#define HAL_SUBHAL_H

#include "HAL/HAL.h"
#include <boost/asio.hpp>

namespace subjugator {
	class SubHAL : public HAL {
		public:
			SubHAL(boost::asio::io_service &ioservice);
	};
}

#endif

