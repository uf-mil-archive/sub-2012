#include "DDSPublisher/ChatMessage.h"
#include "DDSPublisher/ChatMessageSupport.h"
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

	DDSDataReader *reader = participant->create_datareader(topic, DDS_DATAREADER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!reader)
		throw runtime_error("failed to create DataReader");

	ChatMessageDataReader *chatmessagereader = ChatMessageDataReader::narrow(reader);
	if (!chatmessagereader)
		throw runtime_error("failed to narrow to ChatMessageDataReader");

	DDSWaitSet *waitset = new DDSWaitSet();
	DDSStatusCondition* statuscondition = chatmessagereader->get_statuscondition();
	statuscondition->set_enabled_statuses(DDS_DATA_AVAILABLE_STATUS);
	waitset->attach_condition(statuscondition);

	while (true) {
		DDSConditionSeq conditionseq;
		waitset->wait(conditionseq, DDS_DURATION_INFINITE);

		ChatMessageSeq messageseq;
		DDS_SampleInfoSeq infoseq;
		if (chatmessagereader->take(messageseq, infoseq, DDS_LENGTH_UNLIMITED, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE) != DDS_RETCODE_OK)
			throw runtime_error("failed to take from ChatMessageDataReader");

		for (int i=0; i<messageseq.length(); ++i)
			cout << messageseq[i].username << ": " << messageseq[i].message << endl;

		messageseq.unloan();
		infoseq.unloan();
	}
}

