#include <ndds/ndds_cpp.h>
#include "SubMain/Workers/LPOSVSS/SubLPOSVSSWorker.h"
#include "DDSCommanders/LPOSVSSDDSCommander.h"
#include "DDSListeners/LPOSVSSDDSListener.h"
#include "DataObjects/Depth/DepthInfo.h"

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
	LPOSVSSWorker worker(io, 50, false);
	if(!worker.Startup())
		throw new runtime_error("Failed to start LPOSVSS Worker!");

	// Now we need a DDS listener to push all the data up
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (DepthMessageTypeSupport::register_type(participant, DepthMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (IMUMessageTypeSupport::register_type(participant, IMUMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (DVLMessageTypeSupport::register_type(participant, DVLMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (PDStatusMessageTypeSupport::register_type(participant, PDStatusMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (LPOSVSSMessageTypeSupport::register_type(participant, LPOSVSSMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	LPOSVSSDDSCommander commander(worker, participant);

	LPOSVSSDDSListener listener(worker, participant);

	// Start the worker
	io.run();

	// Cleanly shutdown the worker
	worker.Shutdown();
}

