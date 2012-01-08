#ifndef LIBSUB_DDS_DDSEXCEPTION
#define LIBSUB_DDS_DDSEXCEPTION

#include <stdexcept>
#include <ndds/ndds_cpp.h>

namespace subjugator {
	class DDSException : public std::runtime_error {
		public:
			DDSException(const std::string &msg);
			DDSException(const std::string &msg, DDS_ReturnCode_t retcode);
	};
}

#endif

