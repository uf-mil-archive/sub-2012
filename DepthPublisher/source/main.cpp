#include "DDSMessages/DepthMessage.h"
#include "DDSMessages/DepthMessageSupport.h"
#include <unistd.h>
#include <ndds/ndds_cpp.h>
#include <iostream>
#include <stdexcept>

using namespace std;

int main(int argc, char **argv) {
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (DepthMessageTypeSupport::register_type(participant, DepthMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	DDSTopic *topic = participant->create_topic("Depth", DepthMessageTypeSupport::get_type_name(), DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!topic)
		throw runtime_error("Failed to create Topic");

	DDSDataWriter *writer = participant->create_datawriter(topic, DDS_DATAWRITER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!writer)
		throw runtime_error("Failed to create DataWriter");

	DepthMessageDataWriter *chatwriter = DepthMessageDataWriter::narrow(writer);
	if (!chatwriter)
		throw runtime_error("Failed to narrow to CoordDataWriter");

	while (true) {
		sleep(1);

		DepthMessage *msg = DepthMessageTypeSupport::create_data();
        msg->depth = 30;

		chatwriter->write(*msg, DDS_HANDLE_NIL);
	}
}

