#ifndef LIBSUB_DDS_RECEIVER_H
#define LIBSUB_DDS_RECEIVER_H

#include "LibSub/DDS/DDSException.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	template <class MessageT>
	class Receiver : DDSDataReaderListener {
		public:
			typedef MessageT Message;
			typedef typename MessageTraits<MessageT>::DataReader MessageDataReader;
			typedef typename MessageTraits<MessageT>::Seq MessageSeq;

			typedef boost::function<void (const Message &)> ReceiveCallback;
			typedef boost::function<void (int)> WriterCountCallback;

			Receiver(Topic<Message> &topic, const ReceiveCallback &receivecallback, const WriterCountCallback &writercountcallback)
			: topic(topic), receivecallback(receivecallback), writercountcallback(writercountcallback) {
				Participant &participant = topic.getParticipant();
				participant.registerType<Message>();

				DDSDataReader *reader = participant.getDDS().create_datareader(&topic.getDDS(), DDS_DATAREADER_QOS_USE_TOPIC_QOS, NULL, DDS_STATUS_MASK_NONE);
				if (!reader)
					throw DDSException("Failed to create DataReader");

				messagereader = MessageDataReader::narrow(reader);
				if (!messagereader)
					throw DDSException("Failed to narrow to MessageDataReader");

				if (messagereader->set_listener(this, DDS_DATA_AVAILABLE_STATUS | DDS_LIVELINESS_CHANGED_STATUS) != DDS_RETCODE_OK)
					throw DDSException("Failed to set listener on the MessageDataReader");
			}

			~Receiver() {
				topic.getParticipant().getDDS().delete_datareader(messagereader);
			}

			Topic<Message> &getTopic() { return topic; }

		private:
			Topic<Message> &topic;
			ReceiveCallback receivecallback;
			WriterCountCallback writercountcallback;
			MessageDataReader *messagereader;

			virtual void on_data_available(DDSDataReader *unused) {
				MessageSeq messageseq;
				DDS_SampleInfoSeq infoseq;
				if (messagereader->take(messageseq, infoseq, DDS_LENGTH_UNLIMITED, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE) != DDS_RETCODE_OK)
					throw DDSException("Failed to take from DataReader");

				try {
					for (int i=0; i<messageseq.length(); ++i)
						receivecallback(messageseq[i]);

					messagereader->return_loan(messageseq, infoseq);
				} catch (...) {
					messagereader->return_loan(messageseq, infoseq);
					throw;
				}
			}

			virtual void on_liveliness_changed(DDSDataReader *unused, const DDS_LivelinessChangedStatus &status) {
				writercountcallback(status.alive_count);
			}
	};
}

#endif

