#ifndef _SubLPOSVSSWorker_H
#define _SubLPOSVSSWorker_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/SubTranslations.h"
#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/LPOSVSS/SubNavigationComputer.h"
#include "DataObjects/DVL/DVLHighresBottomTrack.h"
#include "DataObjects/IMU/IMUInfo.h"
#include "DataObjects/Depth/DepthInfo.h"
#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include <boost/ref.hpp>

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
		LPOSVSSWorker(boost::asio::io_service& io, int64_t rate, bool useDVL);
		bool Startup();
		void Shutdown();

	protected:
		  void initializeState();
		  void readyState();
		  void standbyState();
		  void emergencyState();
		  void failState();
		  void allState();
	private:

		  std::auto_ptr<NavigationComputer> navComputer;
		  bool useDVL;

		  std::auto_ptr<DVLHighresBottomTrack> dvlInfo;
		  std::auto_ptr<IMUInfo> imuInfo;
		  std::auto_ptr<DepthInfo> depthInfo;


		  void setDepth(const DataObject& dobj);
		  void setIMU(const DataObject& dobj);
		  void setDVL(const DataObject& dobj);
		  void setCurrents(const DataObject& dobj);
		  void setTare(const DataObject& dobj);

		  boost::int64_t getTimestamp(void);

		  boost::mutex lock;
	};
}

#endif // _SubLPOSVSSWorker_H

