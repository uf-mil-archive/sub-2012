#ifndef HAL_TRANSPORTBASE_H
#define HAL_TRANSPORTBASE_H

#include "HAL/Transport.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace subjugator {
	class TransportBase : public Transport {
		public:
			virtual void configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback);

		protected:
			ReadCallback readcallback;
			ErrorCallback errorcallback;
	};

	class ASIOTransportBase : public TransportBase {
		protected:
			void startIOThread();
			void stopIOThread();
			void runCallbackOnIOThread(boost::function<void ()> callback);

			boost::asio::io_service ioservice;
			boost::thread iothread;

		private:
			bool iothread_running;
	};
}

#endif

