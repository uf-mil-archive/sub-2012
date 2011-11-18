#ifndef LIBSUB_WORKER_SIGNALHANDLER_H
#define LIBSUB_WORKER_SIGNALHANDLER_H

#include <boost/asio.hpp>
#include <boost/function.hpp>

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	SignalHandler implements signal handling for ASIO applications.
	SIGINT stops the io_service, while SIGUSR1 causes the reconfigure callback to be invoked.
	A second SIGINT terminates the program as a normal SIGINT would.
	*/

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

	/** @} */
}

#endif

