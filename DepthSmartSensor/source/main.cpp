#include <ndds/ndds_cpp.h>
#include "DepthSmartSensor/SubDepthWorker.h"

#include "DDSListeners/DepthDDSListener.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/weak_ptr.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

int main(int argc, char **argv)
{
	boost::asio::io_service io;

	// We need a worker
	DepthWorker worker(io, 2 /* hz - this is the heartbeat tick rate */);
	if(!worker.Startup())
		throw new runtime_error("Failed to start Depth Worker!");

	// Now we need a listener to push the data up
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (DepthMessageTypeSupport::register_type(participant, DepthMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	DepthDDSListener ddsListener(worker, participant);

	// Start the worker
	io.run();

	worker.Shutdown();
}

