#ifndef HAL_FILETRANSPORT_H
#define HAL_FILETRANSPORT_H

#include "HAL/transport/Transport.h"
#include <string>

namespace subjugator {
	class FileTransport : public Transport {
		public:
			FileTransport();

			virtual const std::string &getName() const;
			virtual Endpoint *makeEndpoint(const std::string &address, const ParamMap &params);

		private:
	};
}

#endif	// HAL_FILETRANSPORT_H

