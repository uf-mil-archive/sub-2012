#ifndef LIBSUB_DDS_DDSBUILDER_H
#define LIBSUB_DDS_DDSBUILDER_H

#include "LibSub/DDS/Participant.h"
#include "LibSub/DDS/Topic.h"
#include "LibSub/DDS/Sender.h"
#include "LibSub/DDS/Receiver.h"
#include "LibSub/Worker/WorkerSignal.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace subjugator {
	template <class MessageT, typename DataT>
	MessageT to_dds(const DataT &data) { BOOST_STATIC_ASSERT(sizeof(MessageT) == 0); }

	template <typename DataT, class MessageT>
	DataT from_dds(const MessageT &msg) { BOOST_STATIC_ASSERT(sizeof(MessageT) == 0); }

	class DDSBuilder {
		public:
			DDSBuilder(boost::asio::io_service &io) : io(io) { }

			template <class MessageT>
			Topic<MessageT> &topic(const std::string &name, int qosflags = TopicQOS::DEFAULT) {
				TopicObj<MessageT> *topic = new TopicObj<MessageT>(participant, name, qosflags);
				objs.push_back(topic);
				return topic->t;
			}

			template <class MessageT, typename DataT>
			void sender(WorkerSignal<DataT> &signal, Topic<MessageT> &topic) {
				objs.push_back(new SignalSenderObj<MessageT, DataT>(signal, topic));
			}

			template <class MessageT, typename DataT>
			void receiver(WorkerMailbox<DataT> &mailbox, Topic<MessageT> &topic) {
				objs.push_back(new MailboxReceiverObj<MessageT, DataT>(mailbox, topic, io));
			}

		private:
			boost::asio::io_service &io;
			Participant participant;

			struct Obj {
				virtual ~Obj() { }
			};

			boost::ptr_vector<Obj> objs;

			template <class Message>
			struct TopicObj : public Obj {
				Topic<Message> t;
				TopicObj(Participant &participant, const std::string &name, int qosflags)
				: t(participant, name, qosflags) { }
			};

			template <class MessageT, typename DataT>
			struct SignalSenderObj : public Obj {
				typedef SignalSenderObj<MessageT, DataT> Self;

				typename WorkerSignal<DataT>::Connection conn;
				Sender<MessageT> sender;

				SignalSenderObj(WorkerSignal<DataT> &signal, Topic<MessageT> &topic)
				: conn(signal.connect(boost::bind(&Self::callback, this, _1))),
				  sender(topic) { }

				void callback(const DataT &obj) {
					sender.send(to_dds<MessageT>(obj));
				}
			};

			template <class MessageT, typename DataT>
			struct MailboxReceiverObj : public Obj {
				typedef MailboxReceiverObj<MessageT, DataT> Self;

				WorkerMailbox<DataT> &mailbox;
				Receiver<MessageT> receiver;
				boost::asio::io_service &io;

				MailboxReceiverObj(WorkerMailbox<DataT> &mailbox, Topic<MessageT> &topic, boost::asio::io_service &io)
				: mailbox(mailbox),
				  receiver(topic, boost::bind(&Self::receiveCallback, this, _1), boost::bind(&Self::writerCountCallback, this, _1)),
				  io(io) { }

				void receiveCallback(const MessageT &msg) {
					io.dispatch(boost::bind(&WorkerMailbox<DataT>::set, &mailbox, from_dds<DataT>(msg)));
				}

				void writerCountCallback(int count) {
					if (count == 0)
						mailbox.clear();
				}
			};
	};
}

#endif
