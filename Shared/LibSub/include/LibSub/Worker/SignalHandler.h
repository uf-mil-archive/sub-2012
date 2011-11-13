#ifndef LIBSUB_WORKER_SIGNALHANDLER_H
#define LIBSUB_WORKER_SIGNALHANDLER_H

#include <boost/asio.hpp>
#include <boost/function.hpp>

namespace subjugator {
	class SignalHandler {
		public:
			typedef boost::function<void ()> ReconfigureCallback;

			SignalHandler(boost::asio::io_service &io, const ReconfigureCallback &reconfcallback=ReconfigureCallback());
			~SignalHandler();

			void start();
			void stop();

		private:
			boost::asio::io_service &io;
			ReconfigureCallback reconfcallback;
			bool gotsigint;

			void ioCallback(int signal);
			void signalReceived(int signal);

			static SignalHandler *globalptr;
			static void globalptrAction(int signal);
	};
}

#endif

