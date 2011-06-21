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
			typedef boost::function<void (const MessageT &message)> Callback;

			DDSReceiver(DDSDomainParticipant *participant, const std::string &topicname, const Callback &callback) {
				topic = participant->create_topic(topicname.c_str(), MessageTypeSupportT::get_type_name(), DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
				if (!topic)
					throw std::runtime_error("Failed to create Topic");

				DDSDataReader *reader = participant->create_datareader(topic, DDS_DATAREADER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
				if (!reader)
					throw std::runtime_error("Failed to create DataReader");

				messagereader = MessageDataReaderT::narrow(reader);
				if (!messagereader)
					throw std::runtime_error("Failed to narrow to IMUMessageDataReader");

				if (messagereader->set_listener(this, DDS_DATA_AVAILABLE_STATUS) != DDS_RETCODE_OK)
					throw std::runtime_error("Failed to set listener on the IMUMessageDataReader");
					
			    this->callback = callback;
			}

			~DDSReceiver() {
				participant->delete_datareader(messagereader);
				participant->delete_topic(topic);
			}

		private:
			DDSDomainParticipant *participant;
			DDSTopic *topic;
			MessageDataReaderT *messagereader;

			Callback callback;

			virtual void on_data_available(DDSDataReader *unused) {
				MessageSeqT messageseq;
				DDS_SampleInfoSeq infoseq;
				if (messagereader->take(messageseq, infoseq, DDS_LENGTH_UNLIMITED, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE) != DDS_RETCODE_OK)
					throw std::runtime_error("Failed to take from DataReader");

				try {
					for (int i=0; i<messageseq.length(); ++i)
						callback(messageseq[i]);

					messageseq.unloan();
					infoseq.unloan();
				} catch (...) {
					messageseq.unloan(); // RTI can't use destructors properly grumble grumble. This is the only way to ensure this won't leak memory in the event of an exception
					infoseq.unloan();
					throw;
				}
			}
	};
}

#endif

