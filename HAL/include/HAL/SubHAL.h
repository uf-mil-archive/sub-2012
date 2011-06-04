#ifndef HAL_SUBHAL_H
#define HAL_SUBHAL_H

#include "HAL/HAL.h"
#include "IOThread.h"

namespace subjugator {
	class SubHAL : public HAL {
		public:
			SubHAL();

			void startIOThread();

		private:
			IOThread iothread;
	};
}

#endif

