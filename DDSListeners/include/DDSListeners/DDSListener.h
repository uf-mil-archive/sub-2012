#ifndef DDSListener_H
#define DDSListener_H

#include <ndds/ndds_cpp.h>
#include <HAL/format/DataObject.h>
#include <stdexcept>

using namespace std;

namespace subjugator
{
	template <class MessageT, class MessageDataWriterT, class MessageTypeSupportT>
	class DDSListener
	{
	public:
		DDSListener(DDSDomainParticipant *part, const std::string &topicName)
		{
			if(part)
				participant = part;

			topic = participant->create_topic(topicName.c_str(), MessageTypeSupportT::get_type_name(), DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
			if (!topic)
				throw runtime_error("Failed to create Topic");

			writer = participant->create_datawriter(topic, DDS_DATAWRITER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
			if (!writer)
				throw runtime_error("Failed to create DataWriter");

			messageWriter = MessageDataWriterT::narrow(writer);
			if (!messageWriter)
				throw runtime_error("Failed to narrow to CoordDataWriter");
		}

		~DDSListener()
		{
			participant->delete_datawriter(writer);
			participant->delete_topic(topic);
		}

		void Publish(DataObject *obj)
		{
			MessageT *msg = MessageTypeSupportT::create_data();
			BuildMessage(msg, obj);
			messageWriter->write(*msg, DDS_HANDLE_NIL);
		}

	protected:
		// This builds the message in place
		virtual void BuildMessage(MessageT *msg, DataObject *obj){}


	private:
		DDSDomainParticipant *participant;
		DDSDataWriter *writer;
		DDSTopic *topic;
		MessageDataWriterT *messageWriter;
	};


}

#endif // DDSListener_H
