#ifndef IMUDDSListener_H
#define IMUDDSListener_H

#include "DataObjects/IMU/IMUDataObjectFormatter.h"
#include "DataObjects/IMU/IMUInfo.h"

#include "DDSMessages/IMUMessage.h"
#include "DDSMessages/IMUMessageSupport.h"
#include "DDSListeners/DDSListener.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class IMUDDSListener : public DDSListener<IMUMessage, IMUMessageDataWriter, IMUMessageTypeSupport>
	{
	public:
		IMUDDSListener(DDSDomainParticipant *part)
		 : DDSListener<IMUMessage, IMUMessageDataWriter, IMUMessageTypeSupport>(part, "IMU") {}

	protected:
		virtual void BuildMessage(IMUMessage *msg, DataObject *obj);
	private:
	};
}

#endif // IMUDDSListener_H
