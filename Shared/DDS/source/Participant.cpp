#include "DDS/Participant.h"

using namespace subjugator;

Participant::Participant() {
	ddsparticipant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!ddsparticipant)
		throw DDSException("Failed to create DDSDomainParticipant");
}

Participant::~Participant() {
	DDSDomainParticipantFactory::get_instance()->delete_participant(ddsparticipant);
	DDSDomainParticipantFactory::finalize_instance();
}

