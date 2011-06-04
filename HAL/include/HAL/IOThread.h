#ifndef HAL_TRANSPORTBASE_H
#define HAL_TRANSPORTBASE_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace subjugator {
	class IOThread {
		public:
			IOThread();

			void start();
			void stop();
			void run(boost::function<void ()> callback);

			inline boost::asio::io_service &getIOService() { return ioservice; }
			inline const boost::asio::io_service &getIOService() const { return ioservice; }

		private:
			bool running;
			boost::asio::io_service ioservice;
			boost::thread iothread;
	};
}

#endif

