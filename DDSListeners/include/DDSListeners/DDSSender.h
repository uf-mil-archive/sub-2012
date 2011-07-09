#ifndef DDSSender_H
#define DDSSender_H

#include "HAL/format/DataObject.h"
#include <ndds/ndds_cpp.h>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <stdexcept>

using namespace std;

namespace subjugator
{
	template <class MessageT, class MessageDataWriterT, class MessageTypeSupportT>
	class DDSSender
	{
	public:
		DDSSender(DDSDomainParticipant *participant, const std::string &topicName)
		: participant(participant)
		{
			topic = participant->create_topic(topicName.c_str(), MessageTypeSupportT::get_type_name(), DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
			if (!topic)
				throw runtime_error("Failed to create Topic");

			DDS_DataWriterQos qos = DDS_DATAWRITER_QOS_DEFAULT;
			qos.liveliness.kind = DDS_AUTOMATIC_LIVELINESS_QOS;
			qos.liveliness.lease_duration.sec = 0;
			qos.liveliness.lease_duration.nanosec = (DDS_UnsignedLong)(.5 * 1E9);

			writer = participant->create_datawriter(topic, qos, NULL, DDS_STATUS_MASK_NONE);
			if (!writer)
				throw runtime_error("Failed to create DataWriter");

			messageWriter = MessageDataWriterT::narrow(writer);
			if (!messageWriter)
				throw runtime_error("Failed to narrow to CoordDataWriter");
		}

		~DDSSender()
		{
			participant->delete_datawriter(writer);
			participant->delete_topic(topic);
		}

		void Send(const MessageT &message)
		{
			messageWriter->write(message, DDS_HANDLE_NIL);
		}


	private:
		DDSDomainParticipant *participant;
		DDSDataWriter *writer;
		DDSTopic *topic;
		MessageDataWriterT *messageWriter;
	};
}

#endif // DDSSender_H
