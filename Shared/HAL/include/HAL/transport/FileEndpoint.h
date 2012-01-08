#ifndef HAL_FILEENDPOINT_H
#define HAL_FILEENDPOINT_H

#include "HAL/transport/BaseStreamEndpoint.h"

namespace subjugator {
	class FileEndpoint : public BaseStreamEndpoint<boost::asio::posix::stream_descriptor> {
		public:
			FileEndpoint(const std::string &filename, boost::asio::io_service &io);

			virtual void open();

		private:
			std::string filename;
	};
}

#endif
