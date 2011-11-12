#ifndef LIBSUB_DDS_DDSBUILDER_H
#define LIBSUB_DDS_DDSBUILDER_H

#include "LibSub/DDS/Participant.h"
#include "LibSub/DDS/Topic.h"
#include "LibSub/DDS/Sender.h"
#include "LibSub/DDS/Receiver.h"
#include "LibSub/DDS/Conversions.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Messages/WorkerLogMessageSupport.h"
#include "LibSub/Messages/WorkerStateMessageSupport.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <utility>

DECLARE_MESSAGE_TRAITS(WorkerLogMessage);
DECLARE_MESSAGE_TRAITS(WorkerStateMessage);

namespace subjugator {
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

			void worker(Worker &worker) {
				sender(worker.logger, topic<WorkerLogMessage>("WorkerLog", TopicQOS::DEEP_PERSISTENT));
				objs.push_back(new WorkerStateSenderObj(worker, topic<WorkerStateMessage>("WorkerStates")));
			}

			~DDSBuilder() {
				while (objs.size())
					objs.pop_back();
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
					MessageT msg;
					to_dds(msg, obj);
					sender.send(msg);
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
					DataT data;
					from_dds(data, msg);
					io.dispatch(boost::bind(&WorkerMailbox<DataT>::set, &mailbox, data));
				}

				void writerCountCallback(int count) {
					if (count == 0)
						mailbox.clear();
				}
			};

			struct WorkerStateSenderObj : public Obj {
				Worker &worker;
				Sender<WorkerStateMessage> statesender;
				Worker::StateChangedSignal::Connection stateconn;

				WorkerStateSenderObj(Worker &worker, Topic<WorkerStateMessage> &statetopic)
				: worker(worker),
				  statesender(statetopic),
				  stateconn(worker.statechangedsig.connect(bind(&WorkerStateSenderObj::stateChangedCallback, this, _1))) { }

				void stateChangedCallback(const State &state) {
					WorkerStateMessage msg;
					to_dds(msg.worker, worker.getName());
					to_dds(msg.state, state);
					statesender.send(msg);
				}
			};
	};
}

#endif
