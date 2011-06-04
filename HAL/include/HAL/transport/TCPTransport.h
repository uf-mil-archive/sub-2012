#ifndef HAL_TCPTRANSPORT_H
#define HAL_TCPTRANSPORT_H

#include "HAL/transport/Transport.h"
#include "HAL/IOThread.h"
#include <utility>
#include <vector>
#include <string>

namespace subjugator {
	class TCPTransport : public Transport {
		public:
			TCPTransport(IOThread &iothread);

			virtual const std::string &getName() const;
			virtual Endpoint *makeEndpoint(const std::string &address, const ParamMap &params);

		private:
			IOThread &iothread;
	};
}

#endif
