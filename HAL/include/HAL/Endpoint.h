#ifndef HAL_ENDPOINT_H
#define HAL_ENDPOINT_H

#include "HAL/shared.h"
#include <boost/function.hpp>
#include <string>

namespace subjugator {
	class Endpoint {
		public:
			virtual ~Endpoint() { }

			typedef boost::function<void (ByteVec::const_iterator begin, ByteVec::const_iterator end)> ReadCallback;
			typedef boost::function<void ()> StateChangeCallback;

			virtual void configureCallbacks(const ReadCallback &readcallback, const StateChangeCallback &statechangecallback) = 0;

			virtual void open() = 0;
			virtual void close() = 0;

			enum State {
				CLOSED,
				OPEN,
				ERROR
			};

			virtual State getState() const = 0;
			virtual const std::string &getErrorMessage() const = 0;

			virtual void write(ByteVec::const_iterator begin, ByteVec::const_iterator end) = 0;
	};
}

#endif

