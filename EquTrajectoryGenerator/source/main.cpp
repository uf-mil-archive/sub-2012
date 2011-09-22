#include <ndds/ndds_cpp.h>
#include "EquTrajectoryGenerator/EquTrajectoryGeneratorWorker.h"
#include "EquTrajectoryGenerator/EquTrajectoryGeneratorDDSCommander.h"
#include "DDSListeners/TrajectoryGeneratorDDSListener.h"
#include "DDSMessages/PDStatusMessageSupport.h"
#include "DDSMessages/SetWaypointMessageSupport.h"
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
	EquTrajectoryGeneratorWorker worker(io, 50);
	if(!worker.Startup())
		throw new runtime_error("Failed to start TrajectoryGeneratorWorker!");

	// Now we need a DDS listener to push all the data up
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (TrajectoryMessageTypeSupport::register_type(participant, TrajectoryMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (PDStatusMessageTypeSupport::register_type(participant, PDStatusMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (SetWaypointMessageTypeSupport::register_type(participant, SetWaypointMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (TrajectoryMessageTypeSupport::register_type(participant, TrajectoryMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (LPOSVSSMessageTypeSupport::register_type(participant, LPOSVSSMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (PDStatusMessageTypeSupport::register_type(participant, PDStatusMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	TrajectoryGeneratorDDSListener listener(worker, participant);
	EquTrajectoryGeneratorDDSCommander commander(worker, participant);

	// Start the worker
	io.run();

	// Cleanly shutdown the worker
	worker.Shutdown();
}

