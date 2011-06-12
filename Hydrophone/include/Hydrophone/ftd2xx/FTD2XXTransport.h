#ifndef HYDROPHONE_FTD2XXTRANSPORT_H
#define HYDROPHONE_FTD2XXTRANSPORT_H

#include "HAL/transport/Transport.h"
#include <string>

namespace subjugator {
	class FTD2XXTransport : public Transport {
		public:
			FTD2XXTransport();

			virtual const std::string &getName() const;
			virtual Endpoint *makeEndpoint(const std::string &address, const ParamMap &params);

		private:
			int findDevice(const std::string &address, const ParamMap &params);
			int findDeviceFromDescription(const std::string &description);
	};
}

#endif

