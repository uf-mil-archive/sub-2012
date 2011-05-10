#include "SubMain/Workers/SubWorker.h"

namespace subjugator
{
	Worker::Worker(boost::asio::io_service& io, int64_t rateHz)
	{
		mRate =  rateHz;
		mMilliseconds = 1000 / rateHz;
		mRateTimer = boost::shared_ptr<boost::asio::deadline_timer>(
			new boost::asio::deadline_timer(
				io, boost::posix_time::milliseconds(mMilliseconds)));

		// When calling bind on a member function, the object to operate on is prepended as an argument, hence the this
		mRateTimer->async_wait(boost::bind(&Worker::work, this, boost::asio::placeholders::error));
	}

	void Worker::work(const boost::system::error_code& /*e*/)
	{
		std::cout << "Working!" << std::endl;

		// Execute calls both all-state and the current state functions
		mStateManager.Execute();

		// Setup to expire again - 1 shot timer hacks
		mRateTimer->expires_at(mRateTimer->expires_at() + boost::posix_time::milliseconds(mMilliseconds));
		mRateTimer->async_wait(boost::bind(&Worker::work, this, boost::asio::placeholders::error));
	}
}
