#ifndef _SubPDWorker_H__
#define _SubPDWorker_H__

#include "SubMain/SubPrerequisites.h"
#include "SubMain/SubBoolTimer.h"
#include "SubMain/SubTranslations.h"
#include "SubMain/Workers/SubWorker.h"
#include "PrimitiveDriver/SubThrusterManager.h"
#include "PrimitiveDriver/MergeManager.h"
#include "HAL/SubHAL.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "HAL/IOThread.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/EmbeddedTypeCodes.h"

namespace subjugator
{
	class PDWorkerCommands
	{
	public:
		enum PDWorkerCommandCode
		{
			SetScrew = 0,
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
		IOThread iothread;
		boost::shared_ptr<SubHAL> hal;
		boost::scoped_ptr<DataObjectEndpoint> hbeatEndpoint;
		std::auto_ptr<ThrusterManager> thrusterManager;
		MergeManager mergeManager;

		void setScrew(const DataObject &obj);
		void setActuator(const DataObject &obj);

		void halMergeReceiveCallback(std::auto_ptr<DataObject> &dobj);
		void halMergeStateChangeCallback();

		BoolTimer stoptimer;
		BoolTimer gotimer;
		bool seconddrop;
	};
}

#endif // _SubPDWorker_H__

