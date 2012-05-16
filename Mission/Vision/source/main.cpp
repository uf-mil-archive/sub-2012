#include <ndds/ndds_cpp.h>
#include "VisionWorker.h"
#include "VisionDDSListener.h"
#include "VisionDDSCommander.h"
#include "DataObjects/Vision/VisionSetIDs.h"
#include "DDSMessages/Finder2DMessageSupport.h"
#include "DDSMessages/Finder3DMessageSupport.h"
#include "DDSMessages/VisionSetIDsMessageSupport.h"
#include <boost/thread.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

int main(int argc, char **argv)
{
	boost::asio::io_service io;

	if (argc < 5) {
		cerr << "Usage: camNum camId shutterVal gainVal showDebugImages logImages objectIDs..." << endl;
		cerr << "Transdec @ 6pm: for: 1,2 - down: 4,2" << endl;
		cerr << "Hotel pool @ night: for: 30,10 - down: 30,10" << endl;
		return 1;
	}
	
	boost::property_tree::ptree cameraDesc;
	cameraDesc.put("type", "camera");
	cameraDesc.put("number", lexical_cast<int>(argv[1]));
	int camnum = lexical_cast<int>(argv[2]);
	float shutterVal = lexical_cast<float>(argv[3]);
	float gainVal = lexical_cast<float>(argv[4]);
	bool showDebugImages = lexical_cast<bool>(argv[5]);
	bool logImages = lexical_cast<bool>(argv[6]);

	printf("Debug images: %d\n",(int)showDebugImages);
	printf("Log images: %d\n",(int)logImages);
	
	// We need a worker
	VisionWorker worker(io, 30 /*hz*/, cameraDesc, camnum, showDebugImages, logImages, shutterVal, gainVal);

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
		
	if (FinderMessageListTypeSupport::register_type(participant, FinderMessageListTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");
		
	if (VisionSetIDsMessageTypeSupport::register_type(participant, VisionSetIDsMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (argc > 7) {
		vector<int> ids;
		for (int arg=7; arg<argc; arg++)
		 	ids.push_back(lexical_cast<int>(argv[arg]));

		boost::weak_ptr<InputToken> token = worker.ConnectToCommand(VisionWorkerCommands::UpdateIDs, 2);
		token.lock()->Operate(VisionSetIDs(camnum, ids));
	}

	VisionDDSListener listener(worker, participant, camnum);
	VisionDDSCommander commander(worker, participant);

	// Start the worker
	io.run();

	worker.Shutdown();
}

