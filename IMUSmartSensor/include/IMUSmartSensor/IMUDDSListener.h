#ifndef IMUDDSListener_H
#define IMUDDSListener_H

#include "DataObjects/IMU/IMUDataObjectFormatter.h"
#include "DataObjects/IMU/IMUInfo.h"

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/IMUMessage.h"
#include "DDSMessages/IMUMessageSupport.h"
#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class IMUDDSListener : public Listener
	{
	public:
		IMUDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<IMUMessage, IMUMessageDataWriter, IMUMessageTypeSupport> ddssender;
	};
}

#endif // IMUDDSListener_H
