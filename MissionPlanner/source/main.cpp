#include <ndds/ndds_cpp.h>

#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"
#include "DDSListeners/MissionPlannerDDSListener.h"
#include "DDSCommanders/MissionPlannerDDSCommander.h"

#include "DDSMessages/FinderMessageListSupport.h"
#include "DDSMessages/SetWaypointMessageSupport.h"
#include "DDSMessages/VisionSetIDsMessageSupport.h"
#include "DDSMessages/LPOSVSSMessageSupport.h"
#include "DDSMessages/PDStatusMessage.h"

#include <boost/scoped_ptr.hpp>
#include <boost/asio.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

int main(int argc, char **argv)
{
	boost::asio::io_service io;

	// We need a worker
	MissionPlannerWorker worker(io, 50 /*hz*/);
	if(!worker.Startup())
		throw new runtime_error("Failed to start MissionPlanner Worker!");

	// Now we need a DDS listener to push all the data up
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (FinderMessageListTypeSupport::register_type(participant, FinderMessageListTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (SetWaypointMessageTypeSupport::register_type(participant, SetWaypointMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (VisionSetIDsMessageTypeSupport::register_type(participant, VisionSetIDsMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");
	
	if (LPOSVSSMessageTypeSupport::register_type(participant, LPOSVSSMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");
		
	if (PDStatusMessageTypeSupport::register_type(participant, PDStatusMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (PDActuatorMessageTypeSupport::register_type(participant, PDActuatorMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	MissionPlannerDDSListener ddsListener(worker, participant);
	MissionPlannerDDSCommander ddsCommander(worker, participant);

	// Start the worker
	io.run();

	// Cleanly shutdown the worker
	worker.Shutdown();
}

