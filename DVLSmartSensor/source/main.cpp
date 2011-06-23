#include <ndds/ndds_cpp.h>
#include "SubMain/Workers/SubDVLWorker.h"

#include "DDSListeners/DVLDDSListener.h"

#include <boost/thread.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

int main(int argc, char **argv)
{
	boost::asio::io_service io;

	// We need a worker
	DVLWorker worker(io, 1 /*hz*/);
	if(!worker.Startup())
		throw new runtime_error("Failed to start DVL Worker!");

	// Now we need a DDS listener to push all the data up
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (DVLMessageTypeSupport::register_type(participant, DVLMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	DVLDDSListener ddsListener(worker, participant);

	// Start the worker
	io.run();

	worker.Shutdown();
}

