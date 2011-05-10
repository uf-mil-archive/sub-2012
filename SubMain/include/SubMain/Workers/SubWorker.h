#ifndef _SubWorker_H__
#define _SubWorker_H__

#include "SubMain/SubPrerequisites.h"

#include "SubMain/SubTranslations.h"
#include "SubMain/SubStateManager.h"

namespace subjugator
{
  class Worker
  {
  public:
	  Worker(boost::asio::io_service& io, int64_t rateHz);

  protected:
	  StateManager mStateManager;

  private:
	  int64_t mRate;
	  int64_t mMilliseconds;

	  boost::shared_ptr<boost::asio::deadline_timer> mRateTimer;

	  void work(const boost::system::error_code& /*e*/);
  };
}

#endif // _SubWorker_H__

