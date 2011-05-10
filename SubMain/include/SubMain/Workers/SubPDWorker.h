#ifndef _SubPDWorker_H__
#define _SubPDWorker_H__

#include "SubMain/SubPrerequisites.h"

#include "SubMain/SubTranslations.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator
{
	class PDWorker : public Worker
	{
	public:
		PDWorker(boost::asio::io_service& io, int64_t rate);

	protected:
		  void startupState();
		  void initializeState();
		  void readyState();
		  void standbyState();
		  void emergencyState();
		  void failState();
		  void shutdownState();
		  void allState();
	};
}

#endif // _SubPDWorker_H__

