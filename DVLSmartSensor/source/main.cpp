#include "HAL/SubHAL.h"
#include "DataObjects/DVL/DVLDataObjectFormatter.h"
#include "DataObjects/DVL/DVLPacketFormatter.h"
#include "DataObjects/DVL/DVLConfiguration.h"
#include "DataObjects/DVL/DVLBreak.h"
#include "DataObjects/DVL/DVLHighresBottomTrack.h"
#include "DataObjects/DVL/DVLBottomTrackRange.h"
#include "DataObjects/DVL/DVLBottomTrack.h"

#include <ndds/ndds_cpp.h>

#include "DDSListeners/DVLDDSListener.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

DVLDDSListener *listener;

void stateChangeCallback()
{

}

void receiveCallback(auto_ptr<DataObject> &dobj)
{
	if (DVLHighresBottomTrack *info = dynamic_cast<DVLHighresBottomTrack *>(dobj.get()))
	{
		listener->Publish(info);
	}
	// Not interested in the other packets
}

int main(int argc, char **argv)
{
	SubHAL hal;

	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (DVLMessageTypeSupport::register_type(participant, DVLMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	listener = new DVLDDSListener(participant);

	scoped_ptr<DataObjectEndpoint> endpoint(hal.openDataObjectEndpoint(50, new DVLDataObjectFormatter(), new DVLPacketFormatter()));

	endpoint->configureCallbacks(receiveCallback, stateChangeCallback);
	endpoint->open();

	if(endpoint->getState() == Endpoint::ERROR)
	{
		cout << endpoint->getErrorMessage() << endl;
		return 1;
	}

	hal.startIOThread();

	endpoint->write(DVLBreak());
	this_thread::sleep(seconds(2));

	endpoint->write(DVLConfiguration(15, 0));

	while (true)
		this_thread::sleep(seconds(1));

	delete(listener);
}

