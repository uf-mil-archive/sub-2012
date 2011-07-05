#include <ndds/ndds_cpp.h>
#include "VisionWorker.h"
#include "DDSListeners/VisionDDSListener.h"
#include "DataObjects/Vision/VisionSetIDs.h"
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
	VisionWorker worker(io, 30 /*hz*/, 1, false);
	if(!worker.Startup())
		throw new runtime_error("Failed to start Vision Worker!");

	// Now we need a DDS listener to push all the data up
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (Finder2DMessageTypeSupport::register_type(participant, Finder2DMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (Finder3DMessageTypeSupport::register_type(participant, Finder3DMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (argc > 1) {
		vector<int> ids;
		for (int arg=1; arg<argc; arg++)
		 	ids.push_back(lexical_cast<int>(argv[arg]));

		boost::weak_ptr<InputToken> token = worker.ConnectToCommand(VisionWorkerCommands::UpdateIDs, 2);
		token.lock()->Operate(VisionSetIDs(VisionSetIDs(ids)));
	}

	VisionDDSListener listener(worker, participant);

	// Start the worker
	io.run();

	worker.Shutdown();
}

