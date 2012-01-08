#ifndef LIBSUB_DDS_PARTICIPANT
#define LIBSUB_DDS_PARTICIPANT

#include "DDS/DDSException.h"
#include "DDS/Traits.h"
#include <ndds/ndds_cpp.h>
#include <string>

namespace subjugator {
	class Participant {
		public:
			Participant();
			~Participant();

			template <class Message>
			void registerType() {
				typedef typename MessageTraits<Message>::TypeSupport MessageTypeSupport;

				if (MessageTypeSupport::register_type(ddsparticipant, MessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
					throw DDSException("Failed to register type " + std::string(MessageTypeSupport::get_type_name()));
			}

			DDSDomainParticipant &getDDS() { return *ddsparticipant; }

		private:
			DDSDomainParticipant *ddsparticipant;

	};
}

#endif

