#ifndef _SubPDWorker_H__
#define _SubPDWorker_H__

#include "SubMain/SubPrerequisites.h"

#include "SubMain/SubTranslations.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator
{
	class PDWorkerCommands
	{
	public:
		enum PDWorkerCommandCode
		{
			SetWrench = 0,
			SetActuator = 1,
		};
	};

	class PDWorker : public Worker
	{
	public:
		PDWorker(boost::asio::io_service& io, int64_t rate);
		bool Startup();

	protected:
		  void initializeState();
		  void readyState();
		  void standbyState();
		  void emergencyState();
		  void failState();
		  void allState();

	private:
		  void setWrench(const DataObject &obj);
		  void setActuator(const DataObject &obj);
	};
}

#endif // _SubPDWorker_H__

