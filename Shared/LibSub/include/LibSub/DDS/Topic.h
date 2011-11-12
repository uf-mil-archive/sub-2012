#ifndef LIBSUB_DDS_DDSTOPIC_H
#define LIBSUB_DDS_DDSTOPIC_H

#include "LibSub/DDS/Traits.h"
#include <ndds/ndds_cpp.h>
#include <string>

namespace subjugator {
	struct TopicQOS {
		enum {
			LEGACY = (1 << 0),
			RELIABLE = (1 << 1), // sends messages reliably
			EXCLUSIVE = (1 << 2), // highest priority sender gets exclusive access
			PERSISTENT = RELIABLE | (1 << 3), // persist and resend most recent sample to new readers automatically (requires a reliable topic)
			DEEP_PERSISTENT = PERSISTENT | (1 << 4),
			LIVELINESS = (1 << 5), // causes Senders to assert their liveliness and Receivers to drop Senders when it stops receiving liveliness assertions

			DEFAULT = RELIABLE | EXCLUSIVE,
		};
	};

	template <class MessageT>
	class Topic {
		public:
			typedef MessageT Message;
			typedef typename MessageTraits<MessageT>::TypeSupport TypeSupport;

			Topic(Participant &participant, const std::string &name, int qosflags = TopicQOS::DEFAULT) : participant(participant) {
				participant.registerType<Message>();

				DDS_TopicQos qos;
				participant.getDDS().get_default_topic_qos(qos);

				if (qosflags != TopicQOS::LEGACY) {
					qos.destination_order.kind = DDS_BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;

					if (qosflags & TopicQOS::RELIABLE)
						qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
					if (qosflags & TopicQOS::EXCLUSIVE)
						qos.ownership.kind = DDS_EXCLUSIVE_OWNERSHIP_QOS;
					if (qosflags & TopicQOS::PERSISTENT) {
						if (qosflags & TopicQOS::DEEP_PERSISTENT)
							qos.history.kind = DDS_KEEP_ALL_HISTORY_QOS;
						qos.durability.kind = DDS_TRANSIENT_LOCAL_DURABILITY_QOS;
					}
					if (qosflags & TopicQOS::LIVELINESS) {
						qos.liveliness.kind = DDS_AUTOMATIC_LIVELINESS_QOS;
						qos.liveliness.lease_duration.sec = 0;
						qos.liveliness.lease_duration.nanosec = 100 * 1E6L; // 100ms
					}
				}

				ddstopic = participant.getDDS().create_topic(name.c_str(), TypeSupport::get_type_name(), qos, NULL, DDS_STATUS_MASK_NONE);
				if (!ddstopic)
					throw DDSException("Failed to create topic '" + name + "'");
			}

			~Topic() { participant.getDDS().delete_topic(ddstopic); }

			Participant &getParticipant() { return participant; }
			std::string getName() const { return ddstopic->get_name(); }

			DDSTopic &getDDS() { return *ddstopic; }

		private:
			Participant &participant;
			DDSTopic *ddstopic;
	};
}

#endif

