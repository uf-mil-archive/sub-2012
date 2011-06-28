#ifndef _SubLPOSVSSWorker_H
#define _SubLPOSVSSWorker_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/SubTranslations.h"
#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/LPOSVSS/SubNavigationComputer.h"


namespace subjugator
{
	class LPOSVSSWorkerCommands
	{
	public:
		enum LPOSVSSWorkerCommandCode
		{
			SetDepth = 0,
			SetIMU = 1,
			SetDVL = 2,
			SetCurrents = 3,
			SetTare = 4,
		};
	};

	class LPOSVSSWorker : public Worker
	{
	public:
		LPOSVSSWorker(boost::asio::io_service& io, int64_t rate);
		bool Startup();

	protected:
		  void initializeState();
		  void readyState();
		  void standbyState();
		  void emergencyState();
		  void failState();
		  void allState();
	private:
		  std::auto_ptr<NavigationComputer> navComputer;
		  void setDepth(const DataObject& obj);
		  void setDepth2(const DataObject& obj);

		  bool change;
		  bool count5;
	};
}

#endif // _SubLPOSVSSWorker_H

