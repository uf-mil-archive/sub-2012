#include <ndds/ndds_cpp.h>
#include "DDSMessages/PDStatusMessage.h"
#include "DDSMessages/PDStatusMessageSupport.h"
#include "SubMain/Workers/PDWorker/SubPDWorker.h"
#include "DDSListeners/PDDDSListener.h"
#include "DDSCommanders/PDDDSCommander.h"

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
	PDWorker worker(io, 50 /*hz*/);
	if(!worker.Startup())
		throw new runtime_error("Failed to start PD Worker!");

	// Now we need a DDS listener to push all the data up
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (PDStatusMessageTypeSupport::register_type(participant, PDStatusMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	PDDDSListener ddsListener(worker, participant);
	PDDDSCommander ddsCommander(worker, participant);

	// Start the worker
	io.run();

	// Cleanly shutdown the worker
	worker.Shutdown();
}

