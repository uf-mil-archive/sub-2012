#ifndef LPOSVSSDDSListener_H
#define LPOSVSSDDSListener_H

#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/LPOSVSSMessage.h"
#include "DDSMessages/LPOSVSSMessageSupport.h"
#include "DDSListeners/DDSSender.h"

#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class LPOSVSSDDSListener : public Listener
	{
	public:
		LPOSVSSDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<LPOSVSSMessage, LPOSVSSMessageDataWriter, LPOSVSSMessageTypeSupport> ddssender;
	};
}

#endif // LPOSVSSDDSListener_H
