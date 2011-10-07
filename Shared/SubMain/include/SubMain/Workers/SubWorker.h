#ifndef SubWorker_H__
#define SubWorker_H__

#include "SubMain/SubPrerequisites.h"

#include "SubMain/SubTranslations.h"
#include "SubMain/SubStateManager.h"
#include "SubMain/Workers/SubInputToken.h"

#include "HAL/format/DataObject.h"

namespace subjugator
{
  class Worker
  {
  public:
	  typedef boost::signals2::signal<void (boost::shared_ptr<DataObject> obj)> emittingsignal_t;
  protected:
	  class TokenHelper
	 	  {
	 	  public:
	 		  TokenHelper(const boost::function<void (const DataObject &obj)> f, const boost::function<void (int cmd)> df):
	 			  currentOwnerPriority(0), func(f), dFunc(df) {}

	 		  boost::mutex lock;
	 		  int currentOwnerPriority;
	 		  boost::shared_ptr<InputToken> token;
	 		  boost::function<void (const DataObject &obj)> func;
	 		  boost::function<void (int cmd)> dFunc;
	 	  };
  public:
	  Worker(boost::asio::io_service& io, int64_t rateHz) : mRate(rateHz)
	  {
			mMilliseconds = 1000 / rateHz;
			mRateTimer = boost::shared_ptr<boost::asio::deadline_timer>(
				new boost::asio::deadline_timer(
					io, boost::posix_time::milliseconds(mMilliseconds)));

			shutdown = false;
			// When calling bind on a member function, the object to operate on is prepended as an argument, hence the this
			mRateTimer->async_wait(boost::bind(&Worker::work, this, boost::asio::placeholders::error));
	  }

	  virtual bool Startup()=0;
	  virtual void Shutdown(){}

	  int getState(){return mStateManager.GetCurrentStateCode();}

	  boost::signals2::connection ConnectToEmitting(boost::function<void (boost::shared_ptr<DataObject> obj)> callback)
	  {
		  return onEmitting.connect(callback);
	  }

	  boost::weak_ptr<InputToken> ConnectToCommand(int cmd, int priority)
	  {
		  // If the command arg is higher than the vector size, ignore the command - it's invalid
		  if((size_t)cmd > mInputTokenList.size())
			  return boost::weak_ptr<InputToken>();

		  // Does this owner have priority?
		  if(mInputTokenList[cmd]->currentOwnerPriority < priority)
		  {
			  mInputTokenList[cmd]->lock.lock();

			  if(mInputTokenList[cmd]->currentOwnerPriority != 0)
				  mInputTokenList[cmd]->token.reset();

			  mInputTokenList[cmd]->token = boost::shared_ptr<InputToken>(
					  new InputToken(cmd, mInputTokenList[cmd]->lock,
							  mInputTokenList[cmd]->func,
							  mInputTokenList[cmd]->dFunc));

			  mInputTokenList[cmd]->lock.unlock();

			  return boost::weak_ptr<InputToken>(mInputTokenList[cmd]->token);
		  }

		  return boost::weak_ptr<InputToken>();
	  }

  protected:
	  StateManager mStateManager;
	  bool shutdown;
	  // The signal that is used to publish to the listeners
	  emittingsignal_t onEmitting;

	  std::vector<boost::shared_ptr<TokenHelper> > mInputTokenList;
	  void setControlToken(int cmd, const boost::function<void (const DataObject &obj)> callback)
	  {
			if((size_t)cmd >= mInputTokenList.size())
			{
				mInputTokenList.resize(cmd+1);
			}

			mInputTokenList[cmd] = boost::shared_ptr<TokenHelper>(new TokenHelper(callback,
					boost::bind(&Worker::disconnect, this, _1)));
	  }

	  void changeControlTokenCallback(int cmd, const boost::function<void (const DataObject &obj)> callback)
	  {
		  mInputTokenList[cmd]->lock.lock();

		  mInputTokenList[cmd]->func = callback;
		  mInputTokenList[cmd]->lock.unlock();
	  }

	  void disconnect(int cmd)	// This can only get called by the owner of the cmd through the InputToken
	  {
		  mInputTokenList[cmd]->token.reset();
		  mInputTokenList[cmd]->currentOwnerPriority = 0;
	  }

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
			if(!shutdown)
			{
				mRateTimer->expires_at(mRateTimer->expires_at() + boost::posix_time::milliseconds(mMilliseconds));
				mRateTimer->async_wait(boost::bind(&Worker::work, this, boost::asio::placeholders::error));
			}
	  }

	protected:
	  boost::int64_t getTimestamp(void)
		{
			timespec t;
			clock_gettime(CLOCK_MONOTONIC, &t);

			return ((long long int)t.tv_sec * 1E9) + t.tv_nsec;
		}
  };
}

#endif // _SubWorker_H__

