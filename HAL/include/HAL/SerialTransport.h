#ifndef HAL_SERIALTRANSPORT_H
#define HAL_SERIALTRANSPORT_H

#include "HAL/Transport.h"
#include "HAL/IOThread.h"
#include <utility>
#include <vector>
#include <string>

namespace subjugator {
	class SerialTransport : public Transport {
		public:
			SerialTransport(IOThread &iothread);

			virtual const std::string &getName() const;
			virtual Endpoint *makeEndpoint(const std::string &address, const ParamMap &params);

		private:
			IOThread &iothread;
	};
}

#endif
