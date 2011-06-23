#ifndef SubWorker_H__
#define SubWorker_H__

#include "SubMain/SubPrerequisites.h"

#include "SubMain/SubTranslations.h"
#include "SubMain/SubStateManager.h"

#include "HAL/format/DataObject.h"

namespace subjugator
{
  class Worker
  {
  public:
	  typedef boost::signals2::signal<void (boost::shared_ptr<DataObject> obj)> emittingsignal_t;
  public:
	  Worker(boost::asio::io_service& io, int64_t rateHz) : mRate(rateHz)
	  {
			mMilliseconds = 1000 / rateHz;
			mRateTimer = boost::shared_ptr<boost::asio::deadline_timer>(
				new boost::asio::deadline_timer(
					io, boost::posix_time::milliseconds(mMilliseconds)));

			// When calling bind on a member function, the object to operate on is prepended as an argument, hence the this
			mRateTimer->async_wait(boost::bind(&Worker::work, this, boost::asio::placeholders::error));
	  }

	  boost::signals2::connection ConnectToEmitting(boost::function<void (boost::shared_ptr<DataObject> obj)> callback)
	  {
		  return onEmitting.connect(callback);
	  }

	  virtual bool Startup(){ return true; }
	  virtual void Shutdown(){}

  protected:
	  StateManager mStateManager;

	  // The signal that is used to publish to the listeners
	  emittingsignal_t onEmitting;

  private:
	  int64_t mRate;
	  int64_t mMilliseconds;

	  // This is the callback timer that calls the current state function
	  boost::shared_ptr<boost::asio::deadline_timer> mRateTimer;
	  void work(const boost::system::error_code& /*e*/)
	  {
			// Execute calls both all-state and the current state functions
			mStateManager.Execute();

			// Setup to expire again - 1 shot timer hacks
			mRateTimer->expires_at(mRateTimer->expires_at() + boost::posix_time::milliseconds(mMilliseconds));
			mRateTimer->async_wait(boost::bind(&Worker::work, this, boost::asio::placeholders::error));
	  }
  };
}

#endif // _SubWorker_H__

