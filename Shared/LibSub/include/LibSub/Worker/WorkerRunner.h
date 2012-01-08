#ifndef LIBSUB_WORKER_WORKERRUNNER_H
#define LIBSUB_WORKER_WORKERRUNNER_H

/**
\addtogroup LibSub
@{
*/

#include "LibSub/Worker/Worker.h"
#include <boost/asio.hpp>
#include <boost/date_time.hpp>

namespace subjugator {
	/**
	\brief Handles the Worker's update loop
	*/

	class WorkerRunner {
		public:
			WorkerRunner(Worker& worker, boost::asio::io_service& io_service);

			void start();
			bool isRunning() const { return running; }

		private:
			void tick(const boost::system::error_code& error);

			Worker &worker;
			const boost::posix_time::time_duration timer_period;
			boost::asio::deadline_timer timer;
			boost::posix_time::ptime prevtime;
			bool running;

			static boost::posix_time::time_duration calculatePeriod(const Worker &worker);
	};
}

/** @} */

#endif

