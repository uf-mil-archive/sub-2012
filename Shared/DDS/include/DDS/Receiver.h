#ifndef LIBSUB_DDS_RECEIVER_H
#define LIBSUB_DDS_RECEIVER_H

#include "DDS/DDSException.h"
#include "DDS/Topic.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace subjugator {
	template <class MessageT>
	class BaseReceiver {
		protected:
			typedef MessageT Message;
			typedef typename MessageTraits<MessageT>::DataReader MessageDataReader;
			typedef typename MessageTraits<MessageT>::Seq MessageSeq;

		public:
			BaseReceiver(Topic<Message> &topic)
			: topic(topic) {
				Participant &participant = topic.getParticipant();
				participant.registerType<Message>();

				DDSDataReader *reader = participant.getDDS().create_datareader(&topic.getDDS(), DDS_DATAREADER_QOS_USE_TOPIC_QOS, NULL, DDS_STATUS_MASK_NONE);
				if (!reader)
					throw DDSException("Failed to create DataReader");

				messagereader = MessageDataReader::narrow(reader);
				if (!messagereader)
					throw DDSException("Failed to narrow to MessageDataReader");
			}

			~BaseReceiver() {
				topic.getParticipant().getDDS().delete_datareader(messagereader);
			}

			Topic<Message> &getTopic() { return topic; }

		protected:
			Topic<Message> &topic;
			MessageDataReader *messagereader;

			struct Loan {
				MessageSeq &msgseq;
				DDS_SampleInfoSeq &infoseq;
				MessageDataReader *messagereader;

				Loan(MessageSeq &msgseq, DDS_SampleInfoSeq &infoseq, MessageDataReader *messagereader) :
				msgseq(msgseq), infoseq(infoseq), messagereader(messagereader) { }

				~Loan() {
					messagereader->return_loan(msgseq, infoseq);
				}
			};
	};

	template <class MessageT>
	class PollingReceiver : public BaseReceiver<MessageT> {
		protected:
			using BaseReceiver<MessageT>::messagereader;
			typedef typename MessageTraits<MessageT>::TypeSupport TypeSupport;

		public:
			PollingReceiver(Topic<MessageT> &topic) : BaseReceiver<MessageT>(topic) { }

			boost::shared_ptr<MessageT> take() {
				typename BaseReceiver<MessageT>::MessageSeq messageseq;
				DDS_SampleInfoSeq infoseq;

				while (true) {
					DDS_ReturnCode_t code = messagereader->take(messageseq, infoseq, 1, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE);
					typename BaseReceiver<MessageT>::Loan loan(messageseq, infoseq, messagereader);

					if (code == DDS_RETCODE_NO_DATA)
						return boost::shared_ptr<MessageT>();
					else if (code != DDS_RETCODE_OK)
						throw DDSException("Failed to take from messagereader", code);

					if (infoseq[0].valid_data)
						return toSharedPtr(messageseq[0]);
				}
			}

			boost::shared_ptr<MessageT> read() {
				typename BaseReceiver<MessageT>::MessageSeq messageseq;
				DDS_SampleInfoSeq infoseq;

				DDS_ReturnCode_t code = messagereader->read(messageseq, infoseq, 1, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ALIVE_INSTANCE_STATE);
				typename BaseReceiver<MessageT>::Loan loan(messageseq, infoseq, messagereader);

				if (code == DDS_RETCODE_NO_DATA)
					return boost::shared_ptr<MessageT>();
				else if (code != DDS_RETCODE_OK)
					throw DDSException("Failed to read from messagereader", code);

				if (infoseq[0].valid_data)
					return toSharedPtr(messageseq[0]);
				else
					return boost::shared_ptr<MessageT>();
			}

			std::vector<boost::shared_ptr<MessageT> > readAll() {
				typename BaseReceiver<MessageT>::MessageSeq messageseq;
				DDS_SampleInfoSeq infoseq;
				std::vector<boost::shared_ptr<MessageT> > outvec;

				DDS_ReturnCode_t code = messagereader->read(messageseq, infoseq, DDS_LENGTH_UNLIMITED, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ALIVE_INSTANCE_STATE);
				if (code != DDS_RETCODE_OK && code != DDS_RETCODE_NO_DATA)
					throw DDSException("Failed to read from messagereader", code);

				outvec.resize(messageseq.length());
				for (int i=0; i<messageseq.length(); ++i) {
					outvec[i] = toSharedPtr(messageseq[i]);
				}

				messagereader->return_loan(messageseq, infoseq);
				return outvec;
			}

		private:
			boost::shared_ptr<MessageT> toSharedPtr(const MessageT &msg) {
				boost::shared_ptr<MessageT> ptr(TypeSupport::create_data(), &TypeSupport::delete_data);
				TypeSupport::copy_data(ptr.get(), &msg);
				return ptr;
			}
	};

	template <class MessageT>
	class BlockingReceiver : public PollingReceiver<MessageT> {
		using PollingReceiver<MessageT>::messagereader;

		public:
			BlockingReceiver(Topic<MessageT> &topic) : PollingReceiver<MessageT>(topic) {
				DDSStatusCondition *cond = messagereader->get_statuscondition();
				cond->set_enabled_statuses(DDS_DATA_AVAILABLE_STATUS);
				waitset.attach_condition(cond);
			}

			void wait() {
				DDSConditionSeq seq;
				do {
					waitset.wait(seq, DDS_DURATION_INFINITE);
				} while (!seq.length());
			}

		private:
			DDSWaitSet waitset;
	};

	template <class MessageT>
	class Receiver : public BaseReceiver<MessageT>, DDSDataReaderListener {
		using BaseReceiver<MessageT>::messagereader;

		public:
			typedef boost::function<void (const MessageT &)> ReceiveCallback;
			typedef boost::function<void (const MessageT &)> MessageLostCallback;

			Receiver(Topic<MessageT> &topic, const ReceiveCallback &receivecallback, const MessageLostCallback &messagelostcallback) :
			BaseReceiver<MessageT>(topic), receivecallback(receivecallback), messagelostcallback(messagelostcallback) {
				DDS_ReturnCode_t code = messagereader->set_listener(this, DDS_DATA_AVAILABLE_STATUS);
				if (code != DDS_RETCODE_OK)
					throw DDSException("Failed to set listener on the MessageDataReader", code);
			}

		private:
			ReceiveCallback receivecallback;
			MessageLostCallback messagelostcallback;

			virtual void on_data_available(DDSDataReader *unused) {
				typename BaseReceiver<MessageT>::MessageSeq messageseq;
				DDS_SampleInfoSeq infoseq;
				DDS_ReturnCode_t code;

				code = messagereader->take(messageseq, infoseq, DDS_LENGTH_UNLIMITED, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE);
				if (code != DDS_RETCODE_OK)
					throw DDSException("Failed to take from DataReader", code);

				try {
					for (int i=0; i<messageseq.length(); ++i) {
						if (infoseq[i].valid_data)
							receivecallback(messageseq[i]);
						else if (infoseq[i].instance_state != DDS_ALIVE_INSTANCE_STATE)
							messagelostcallback(messageseq[i]);
					}

					messagereader->return_loan(messageseq, infoseq);
				} catch (...) {
					messagereader->return_loan(messageseq, infoseq);
					throw;
				}
			}
	};
}

#endif

