#ifndef LIBSUB_DDS_SENDER_H
#define LIBSUB_DDS_SENDER_H

#include "LibSub/DDS/Traits.h"
#include <ndds/ndds_cpp.h>

namespace subjugator {
	template <class MessageT>
	class Sender {
		public:
			typedef MessageT Message;
			typedef typename MessageTraits<Message>::DataWriter MessageDataWriter;

			Sender(Topic<Message> &topic) : topic(topic) {
				DDSDataWriter *writer = topic.getParticipant().getDDS().create_datawriter(&topic.getDDS(), DDS_DATAWRITER_QOS_USE_TOPIC_QOS, NULL, DDS_STATUS_MASK_NONE);
				if (!writer)
					throw DDSException("Failed to create DataWriter");

				messagewriter = MessageDataWriter::narrow(writer);
				if (!messagewriter)
					throw DDSException("Failed to narrow DataWriter");
			}

			~Sender() {
				topic.getParticipant().getDDS().delete_datawriter(messagewriter);
			}

			void send(const Message &message) {
				messagewriter->write(message, DDS_HANDLE_NIL);
			}

			Topic<Message> &getTopic() { return topic; }

		private:
			Topic<Message> &topic;
			MessageDataWriter *messagewriter;
	};
}

#endif // DDSSender_H
