#include "DepthSmartSensor/HeartBeatSender.h"

#include "HAL/SubHAL.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "DataObjects/Depth/DepthDataObjectFormatter.h"
#include "DataObjects/Depth/DepthInfo.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include "DataObjects/EmbeddedAddresses.h"
#include "DataObjects/HeartBeat.h"

#include <ndds/ndds_cpp.h>

#include "DDSListeners/DepthDDSListener.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

DataObjectEndpoint *endpoint;
HeartBeatSender *heartbeatsender;

DepthDDSListener *listener;

void stateChangeCallback()
{
	if (endpoint->getState() == Endpoint::OPEN) {
		endpoint->write(HeartBeat());
		endpoint->write(StartPublishing(100));

		heartbeatsender->start();
	} else {
		heartbeatsender->stop();
	}
}

void receiveCallback(auto_ptr<DataObject> &dobj)
{
	if (DepthInfo *info = dynamic_cast<DepthInfo *>(dobj.get()))
	{
		listener->Publish(info);
	}
}

int main(int argc, char **argv)
{
	SubHAL hal;

	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (DepthMessageTypeSupport::register_type(participant, DepthMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	listener = new DepthDDSListener(participant);



	endpoint = hal.openDataObjectEndpoint(4, new DepthDataObjectFormatter(DEPTH_ADDR, GUMSTIX_ADDR, DEPTH), new Sub7EPacketFormatter());
	heartbeatsender = new HeartBeatSender(hal.getIOService(), *endpoint, 2);

	endpoint->configureCallbacks(receiveCallback, stateChangeCallback);
	endpoint->open();

	if(endpoint->getState() == Endpoint::ERROR)
	{
		cout << endpoint->getErrorMessage() << endl;
		return 1;
	}

	hal.startIOThread();

	while (true)
		this_thread::sleep(seconds(1));

	delete(listener);
}

