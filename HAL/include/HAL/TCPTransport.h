#ifndef HAL_TCPTRANSPORT_H
#define HAL_TCPTRANSPORT_H

#include "HAL/Transport.h"
#include "HAL/IOThread.h"
#include <utility>
#include <vector>
#include <string>

namespace subjugator {
	class TCPTransport : public Transport {
		public:
			TCPTransport();

			virtual const std::string &getName() const;
			virtual Endpoint *makeEndpoint(const std::string &address, std::map<std::string, std::string> params);

		private:
			IOThread iothread;
	};
}

#endif
