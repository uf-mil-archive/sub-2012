#include <ndds/ndds_cpp.h>
#include "SubMain/Workers/TrackingController/TrackingControllerWorker.h"
#include "DDSCommanders/TrackingControllerDDSCommander.h"
#include "DDSListeners/TrackingControllerDDSListener.h"
#include "DDSMessages/SetWaypointMessage.h"
#include "DDSMessages/SetWaypointMessageSupport.h"

#include "DDSCommanders/LPOSVSSDDSCommander.h"
#include "DDSListeners/LPOSVSSDDSListener.h"
#include "DDSMessages/LPOSVSSMessage.h"
#include "DDSMessages/LPOSVSSMessageSupport.h"
#include "DDSMessages/TrackingControllerLogMessage.h"
#include "DDSMessages/TrackingControllerLogMessageSupport.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <fstream>

#include <Eigen/Dense>

using namespace subjugator;
using namespace boost;
using namespace std;
using namespace Eigen;

int main(int argc, char **argv)
{
	boost::asio::io_service io;

	// We need a worker
	TrackingControllerWorker worker(io, 50);
	if(!worker.Startup())
		throw new runtime_error("Failed to start TrackingController Worker!");

	// Now we need a DDS listener to push all the data up
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (SetWaypointMessageTypeSupport::register_type(participant, SetWaypointMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (PDWrenchMessageTypeSupport::register_type(participant, PDWrenchMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
			throw runtime_error("Failed to register type");

	if (TrajectoryMessageTypeSupport::register_type(participant, TrajectoryMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
				throw runtime_error("Failed to register type");

	if (LPOSVSSMessageTypeSupport::register_type(participant, LPOSVSSMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
					throw runtime_error("Failed to register type");

	if (PDStatusMessageTypeSupport::register_type(participant, PDStatusMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
					throw runtime_error("Failed to register type");

	if (ControllerGainsMessageTypeSupport::register_type(participant, ControllerGainsMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
						throw runtime_error("Failed to register type");

	if (TrackingControllerLogMessageTypeSupport::register_type(participant, TrackingControllerLogMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	TrackingControllerDDSCommander commander(worker, participant);

	TrackingControllerDDSListener listener(worker, participant);


	// Start the worker
	io.run();

	// Cleanly shutdown the worker
	worker.Shutdown();
}

