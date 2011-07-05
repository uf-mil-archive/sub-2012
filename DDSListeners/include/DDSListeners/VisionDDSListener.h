#ifndef DDSLISTENERS_VISIONDDSLISTENER_H
#define DDSLISTENERS_VISIONDDSLISTENER_H

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/Finder2DMessage.h"
#include "DDSMessages/Finder2DMessageSupport.h"
#include "DDSMessages/Finder3DMessage.h"
#include "DDSMessages/Finder3DMessageSupport.h"
#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class VisionDDSListener : public Listener
	{
	public:
		VisionDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<Finder2DMessage, Finder2DMessageDataWriter, Finder2DMessageTypeSupport> finder2dsender;
		DDSSender<Finder3DMessage, Finder3DMessageDataWriter, Finder3DMessageTypeSupport> finder3dsender;
	};
}

#endif

