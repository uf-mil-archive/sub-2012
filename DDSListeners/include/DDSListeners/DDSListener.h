#ifndef DDSListener_H
#define DDSListener_H

#include "HAL/format/DataObject.h"
#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include <ndds/ndds_cpp.h>

#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

using namespace std;

namespace subjugator
{
	template <class MessageT, class MessageDataWriterT, class MessageTypeSupportT>
	class DDSListener : Listener
	{
	public:
		DDSListener(Worker &worker, DDSDomainParticipant *part, const std::string &topicName)
			: Listener(worker, boost::bind(&DDSListener::Publish, this, _1))
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
			listenConnection.disconnect();
			participant->delete_datawriter(writer);
			participant->delete_topic(topic);
		}

		void Publish(boost::shared_ptr<DataObject> obj)
		{
			MessageT *msg = MessageTypeSupportT::create_data();
			BuildMessage(msg, obj.get());
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
		boost::signals2::connection listenConnection;
	};
}

#endif // DDSListener_H
