#ifndef HAL_TRANSPORT_H
#define HAL_TRANSPORT_H

#include "HAL/shared.h"
#include <boost/function.hpp>
#include <string>

namespace subjugator {
	class Transport {
		public:
			virtual ~Transport() { }

			typedef boost::function<void (int endnum, const ByteVec &bytes)> ReadCallback;
			typedef boost::function<void (int endnum, const std::string &msg)> ErrorCallback; // endnum can be -1 if an error occured but it cannot be traced to a specific endpoint

			virtual void configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback) = 0;
			virtual void start() = 0;
			virtual void stop() = 0;

			virtual int getEndpointCount() const = 0;

			virtual void write(int endnum, const ByteVec &bytes) = 0;
	};
}

#endif

