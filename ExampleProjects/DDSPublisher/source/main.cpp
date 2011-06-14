#include "DDSMessages/ChatMessage.h"
#include "DDSMessages/ChatMessageSupport.h"
#include <ndds/ndds_cpp.h>
#include <iostream>
#include <stdexcept>

using namespace std;

int main(int argc, char **argv) {
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (ChatMessageTypeSupport::register_type(participant, ChatMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	DDSTopic *topic = participant->create_topic("Chat", ChatMessageTypeSupport::get_type_name(), DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!topic)
		throw runtime_error("Failed to create Topic");

	DDSDataWriter *writer = participant->create_datawriter(topic, DDS_DATAWRITER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!writer)
		throw runtime_error("Failed to create DataWriter");

	ChatMessageDataWriter *chatwriter = ChatMessageDataWriter::narrow(writer);
	if (!chatwriter)
		throw runtime_error("Failed to narrow to CoordDataWriter");

	string username;
	cout << "Username: ";
	cin >> username;

	while (true) {
		string line;
		cout << ": ";
		cin >> line;

		ChatMessage *msg = ChatMessageTypeSupport::create_data();
		strcpy(msg->username, username.c_str());
		strcpy(msg->message, line.c_str());

		chatwriter->write(*msg, DDS_HANDLE_NIL);
	}
}

