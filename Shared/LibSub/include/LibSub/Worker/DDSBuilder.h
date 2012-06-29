#ifndef LIBSUB_DDS_DDSBUILDER_H
#define LIBSUB_DDS_DDSBUILDER_H

#include "DDS/Participant.h"
#include "DDS/Topic.h"
#include "DDS/Sender.h"
#include "DDS/Receiver.h"
#include "DDS/Conversions.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include "LibSub/Messages/WorkerLogMessageSupport.h"
#include "LibSub/Messages/WorkerStateMessageSupport.h"
#include "LibSub/Messages/WorkerKillMessageSupport.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <utility>
#include <memory>
#include <cassert>

DECLARE_MESSAGE_TRAITS(WorkerLogMessage);
DECLARE_MESSAGE_TRAITS(WorkerStateMessage);
DECLARE_MESSAGE_TRAITS(WorkerKillMessage);

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	DDSBuilder is a utility class for quickly setting up connections between DDS and a Worker.
	It understands WorkerMailboxes and WorkerSignals, and can automatically set up Senders
	and Receivers in a type safe fashion. It also automatically cleans up all DDS objects
	it creates.
	*/

	class DDSBuilder {
		public:
			DDSBuilder(boost::asio::io_service &io) : io(io) { }

			/**
			#topic generates a topic with a given name and qos. DDSBuilder retains ownership
			of the topic.
			*/

			template <class MessageT>
			Topic<MessageT> &topic(const std::string &name, int qosflags = TopicQOS::DEFAULT) {
				TopicMap::iterator i = topicobjs.find(name);

				if (i != topicobjs.end()) {
					TopicObj<MessageT> &topic = dynamic_cast<TopicObj<MessageT> &>(*i->second);
					assert(topic.t.getQOSFlags() == qosflags);
					return topic.t;
				} else {
					TopicObj<MessageT> *topic = new TopicObj<MessageT>(participant, name, qosflags);
					topicobjs.insert(name, std::auto_ptr<TopicObj<MessageT> >(topic));
					return topic->t;
				}
			}

			/**
			#sender generates a Sender from a WorkerSignal, which will send to the given
			topic. #to_dds must be specialized for MessageT and DataT.
			*/

			template <class MessageT, typename DataT>
			void sender(WorkerSignal<DataT> &signal, Topic<MessageT> &topic) {
				objs.push_back(new SignalSenderObj<MessageT, DataT>(signal, topic));
			}

			/**
			#receiver generates a Receiver from a WorkerMailbox, which will receive from
			a topic. #from_dds must be specialized for MessageT and DataT.
			*/

			template <class MessageT, typename DataT>
			void receiver(WorkerMailbox<DataT> &mailbox, Topic<MessageT> &topic) {
				objs.push_back(new MailboxReceiverObj<MessageT, DataT>(mailbox, topic, io));
			}

			/**
			#map generates a Receiver from a WorkerMap, which will receive from a topic
			and group the received messages by its key callback. #from_dds must be specialized for ValueT and MessageT.
			*/

			template <class MessageT, typename KeyT, typename ValueT>
			void map(WorkerMap<KeyT, ValueT> &map, Topic<MessageT> &topic) {
				objs.push_back(new MapReceiverObj<MessageT, KeyT, ValueT>(map, topic, io));
			}

			/**
			#worker automatically sets up all parts of DDS that are expected of any worker.
			These are a sender for the WorkerLogger, and a sender for the Worker's state changed signal.
			*/

			void worker(Worker &worker) {
				sender(worker.logger, topic<WorkerLogMessage>("WorkerLog", TopicQOS::DEEP_PERSISTENT));
				objs.push_back(new WorkerStateSenderObj(worker, topic<WorkerStateMessage>("WorkerState", TopicQOS::PERSISTENT | TopicQOS::LIVELINESS)));
			}

			/**
			#killSignal automatically sets up a sender for a WorkerKillSignal, using the correct topic and TopicQOS.
			*/

			void killSignal(WorkerKillSignal &killsig) {
				sender(killsig, topic<WorkerKillMessage>("WorkerKill", TopicQOS::PERSISTENT | TopicQOS::LIVELINESS));
			}

			/**
			#killSignal automatically sets up a receiver for a WorkerKillMonitor, using the correct topic and TopicQOS.
			*/

			void killMonitor(WorkerKillMonitor &killmon) {
				map(killmon, topic<WorkerKillMessage>("WorkerKill", TopicQOS::PERSISTENT | TopicQOS::LIVELINESS));
			}

			~DDSBuilder() {
				while (objs.size())
					objs.pop_back(); // ensure destructors are invoked in reverse order of construction
			}

		private:
			boost::asio::io_service &io;
			Participant participant;

			struct Obj {
				virtual ~Obj() { }
			};

			typedef boost::ptr_map<std::string, Obj> TopicMap;
			TopicMap topicobjs;
			boost::ptr_vector<Obj> objs;

			template <class Message>
			struct TopicObj : public Obj {
				Topic<Message> t;
				TopicObj(Participant &participant, const std::string &name, int qosflags) :
				t(participant, name, qosflags) { }
			};

			template <class MessageT, typename DataT>
			struct SignalSenderObj : public Obj {
				typedef SignalSenderObj<MessageT, DataT> Self;

				typename WorkerSignal<DataT>::Connection conn;
				Sender<MessageT> sender;

				SignalSenderObj(WorkerSignal<DataT> &signal, Topic<MessageT> &topic) :
				conn(signal.connect(boost::bind(&Self::callback, this, _1))),
				sender(topic) { }

				void callback(const DataT &obj) {
					MessageWrapper<MessageT> msg;
					to_dds(*msg, obj);
					sender.send(*msg);
				}
			};

			template <class MessageT, typename DataT>
			struct MailboxReceiverObj : public Obj {
				typedef MailboxReceiverObj<MessageT, DataT> Self;

				WorkerMailbox<DataT> &mailbox;
				Receiver<MessageT> receiver;
				boost::asio::io_service &io;

				MailboxReceiverObj(WorkerMailbox<DataT> &mailbox, Topic<MessageT> &topic, boost::asio::io_service &io) :
				mailbox(mailbox),
				receiver(topic, boost::bind(&Self::receiveCallback, this, _1), boost::bind(&Self::messageLostCallback, this, _1)),
				io(io) { }

				void receiveCallback(const MessageT &msg) {
					DataT data;
					from_dds(data, msg);
					io.dispatch(boost::bind(&WorkerMailbox<DataT>::set, &mailbox, data));
				}

				void messageLostCallback(const MessageT &msg) {
					io.dispatch(boost::bind(&WorkerMailbox<DataT>::clear, &mailbox));
				}
			};

			template <class MessageT, typename KeyT, typename ValueT>
			struct MapReceiverObj : public Obj {
				typedef MapReceiverObj<MessageT, KeyT, ValueT> Self;

				WorkerMap<KeyT, ValueT> &map;
				Receiver<MessageT> receiver;
				boost::asio::io_service &io;

				MapReceiverObj(WorkerMap<KeyT, ValueT> &map, Topic<MessageT> &topic, boost::asio::io_service &io) :
				map(map),
				receiver(topic, boost::bind(&Self::receiveCallback, this, _1), boost::bind(&Self::messageLostCallback, this, _1)),
				io(io) { }

				void receiveCallback(const MessageT &msg) {
					ValueT data;
					from_dds(data, msg);
					io.dispatch(boost::bind(&WorkerMap<KeyT, ValueT>::update, &map, data));
				}

				void messageLostCallback(const MessageT &msg) {
					ValueT data;
					from_dds(data, msg);
					io.dispatch(boost::bind(&WorkerMap<KeyT, ValueT>::remove, &map, data));
				}
			};

			struct WorkerStateSenderObj : public Obj {
				Worker &worker;
				Sender<WorkerStateMessage> statesender;
				Worker::StateChangedSignal::Connection stateconn;

				WorkerStateSenderObj(Worker &worker, Topic<WorkerStateMessage> &statetopic) :
				worker(worker),
				statesender(statetopic),
				stateconn(worker.statechangedsig.connect(bind(&WorkerStateSenderObj::stateChangedCallback, this, _1))) { }

				void stateChangedCallback(const State &state) {
					MessageWrapper<WorkerStateMessage> msg;
					to_dds(msg->worker, worker.getName());
					to_dds(msg->state, state);
					statesender.send(*msg);
				}
			};
	};
}

#endif
