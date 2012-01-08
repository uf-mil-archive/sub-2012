#ifndef LIBSUB_DDS_DDSTOPIC_H
#define LIBSUB_DDS_DDSTOPIC_H

#include "DDS/Traits.h"
#include <ndds/ndds_cpp.h>
#include <string>

namespace subjugator {
	struct TopicQOSBits {
		enum {
			LEGACY = (1 << 0),
			RELIABLE = (1 << 1),
			EXCLUSIVE = (1 << 2),
			PERSISTENT = (1 << 3),
			DEEP_HISTORY = (1 << 4),
			LIVELINESS = (1 << 5)
		};
	};

	struct TopicQOS {
		enum {
			LEGACY = TopicQOSBits::LEGACY,

			UNRELIABLE = 0,
			RELIABLE = TopicQOSBits::RELIABLE, // sends messages reliably
			EXCLUSIVE = TopicQOSBits::EXCLUSIVE, // highest priority sender gets exclusive access
			LIVELINESS = TopicQOSBits::LIVELINESS, // dead writers are detected and their values disposed

			PERSISTENT = TopicQOSBits::RELIABLE | TopicQOSBits::PERSISTENT, // persist and resend most recent sample to new readers automatically
			DEEP_PERSISTENT = TopicQOSBits::RELIABLE | TopicQOSBits::PERSISTENT | TopicQOSBits::DEEP_HISTORY, // persist and resend all samples to new readers automatically

			DEFAULT = RELIABLE | EXCLUSIVE | LIVELINESS
		};
	};

	template <class MessageT>
	class Topic {
		public:
			typedef MessageT Message;
			typedef typename MessageTraits<MessageT>::TypeSupport TypeSupport;

			Topic(Participant &participant, const std::string &name, int qosflags = TopicQOS::DEFAULT) :
			participant(participant), qosflags(qosflags) {
				participant.registerType<Message>();

				DDS_TopicQos qos;
				participant.getDDS().get_default_topic_qos(qos);

				if (qosflags != TopicQOSBits::LEGACY) {
					qos.destination_order.kind = DDS_BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;

					if (qosflags & TopicQOSBits::RELIABLE)
						qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
					if (qosflags & TopicQOSBits::EXCLUSIVE)
						qos.ownership.kind = DDS_EXCLUSIVE_OWNERSHIP_QOS;
					if (qosflags & TopicQOSBits::PERSISTENT) {
						if (qosflags & TopicQOSBits::DEEP_HISTORY)
							qos.history.kind = DDS_KEEP_ALL_HISTORY_QOS;
						qos.durability.kind = DDS_TRANSIENT_LOCAL_DURABILITY_QOS;
					}
					if (qosflags & TopicQOSBits::LIVELINESS) {
						qos.liveliness.lease_duration.sec = 0;
						qos.liveliness.lease_duration.nanosec = 500E6; // 500ms
					}
				}

				ddstopic = participant.getDDS().create_topic(name.c_str(), TypeSupport::get_type_name(), qos, NULL, DDS_STATUS_MASK_NONE);
				if (!ddstopic)
					throw DDSException("Failed to create topic '" + name + "'");
			}

			~Topic() { participant.getDDS().delete_topic(ddstopic); }

			Participant &getParticipant() { return participant; }
			std::string getName() const { return ddstopic->get_name(); }
			int getQOSFlags() const { return qosflags; }

			DDSTopic &getDDS() { return *ddstopic; }

		private:
			Participant &participant;
			DDSTopic *ddstopic;
			int qosflags;
	};
}

#endif

