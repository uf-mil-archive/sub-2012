#ifndef SENSORLOGGER_DDSRECEIVER_H
#define SENSORLOGGER_DDSRECEIVER_H

#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>
#include <string>
#include <stdexcept>

// template class for simple usage of DDS

namespace subjugator {
	template <class MessageT, class MessageDataReaderT, class MessageTypeSupportT, class MessageSeqT>
	class DDSReceiver : DDSDataReaderListener {
		public:
			typedef boost::function<void (const MessageT &message)> MessageCallback;
			typedef boost::function<void (int count)> WriterCountChangedCallback;

			DDSReceiver(DDSDomainParticipant *participant, const std::string &topicname, const MessageCallback &messagecallback, const WriterCountChangedCallback &writercountchangedcallback=WriterCountChangedCallback())
			: participant(participant), messagecallback(messagecallback), writercountchangedcallback(writercountchangedcallback) {
				topic = participant->create_topic(topicname.c_str(), MessageTypeSupportT::get_type_name(), DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
				if (!topic)
					throw std::runtime_error("Failed to create Topic" + topicname);

				/*DDS_DataReaderQos qos = DDS_DATAREADER_QOS_DEFAULT;
				qos.liveliness.kind = DDS_AUTOMATIC_LIVELINESS_QOS;
				qos.liveliness.lease_duration.sec = 0;
				qos.liveliness.lease_duration.nanosec = (DDS_UnsignedLong)(.5 * 1E9);*/
				DDSDataReader *reader = participant->create_datareader(topic, DDS_DATAREADER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
				if (!reader)
					throw std::runtime_error("Failed to create DataReader");

				messagereader = MessageDataReaderT::narrow(reader);
				if (!messagereader)
					throw std::runtime_error("Failed to narrow to MessageDataReader");

				if (messagereader->set_listener(this, DDS_DATA_AVAILABLE_STATUS | DDS_LIVELINESS_CHANGED_STATUS) != DDS_RETCODE_OK)
					throw std::runtime_error("Failed to set listener on the MessageDataReader");
			}

			~DDSReceiver() {
				participant->delete_datareader(messagereader);
				participant->delete_topic(topic);
			}

		private:
			DDSDomainParticipant *participant;
			DDSTopic *topic;
			MessageDataReaderT *messagereader;

			MessageCallback messagecallback;
			WriterCountChangedCallback writercountchangedcallback;

			virtual void on_data_available(DDSDataReader *unused) {
				MessageSeqT messageseq;
				DDS_SampleInfoSeq infoseq;
				if (messagereader->take(messageseq, infoseq, DDS_LENGTH_UNLIMITED, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE) != DDS_RETCODE_OK)
					throw std::runtime_error("Failed to take from DataReader");

				try {
					for (int i=0; i<messageseq.length(); ++i)
						messagecallback(messageseq[i]);

					messagereader->return_loan(messageseq, infoseq);
				} catch (...) {
					messagereader->return_loan(messageseq, infoseq);
					throw;
				}
			}

			virtual void on_liveliness_changed(DDSDataReader *unused, const DDS_LivelinessChangedStatus &status) {
				if (writercountchangedcallback)
					writercountchangedcallback(status.alive_count);
			}
	};
}

#endif

