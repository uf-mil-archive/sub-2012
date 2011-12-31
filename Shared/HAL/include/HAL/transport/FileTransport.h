#ifndef HAL_FILETRANSPORT_H
#define HAL_FILETRANSPORT_H

#include "HAL/transport/Transport.h"
#include <boost/asio.hpp>
#include <string>

namespace subjugator {
	class FileTransport : public Transport {
		public:
			FileTransport(boost::asio::io_service &io);

			virtual const std::string &getName() const;
			virtual Endpoint *makeEndpoint(const std::string &address, const ParamMap &params);

		private:
			boost::asio::io_service &io;
	};
}

#endif	// HAL_FILETRANSPORT_H

