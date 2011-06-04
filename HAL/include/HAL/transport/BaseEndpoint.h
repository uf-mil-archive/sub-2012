#ifndef HAL_BASEENDPOINT_H
#define HAL_BASEENDPOINT_H

#include "HAL/transport/Endpoint.h"
#include "HAL/IOThread.h"
#include <string>

namespace subjugator {
	class BaseEndpoint : public Endpoint {
		public:
			BaseEndpoint(IOThread &iothread);

			virtual void configureCallbacks(const ReadCallback &readcallback, const StateChangeCallback &statechangecallback);
			virtual State getState() const { return state; }
			virtual const std::string &getErrorMessage() const;

		protected:
			void callReadCallback(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			void setState(State state, const std::string &errmsg="");

			IOThread &iothread;

		private:
			ReadCallback readcallback;
			StateChangeCallback statechangecallback;

			State state;
			std::string errmsg;
	};
}

#endif

