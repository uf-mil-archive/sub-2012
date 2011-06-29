#include "MotorCalibrate/IMUSensor.h"
#include <boost/bind.hpp>
#include <stdexcept>

using namespace subjugator;
using namespace boost;
using namespace std;

IMUSensor::IMUSensor() : imureceiver(dds.participant, "IMU", bind(&IMUSensor::ddsCallback, this, _1)) {
}

void IMUSensor::ddsCallback(const IMUMessage &msg) {
	this->msg = msg;
	emit onNewMessage();
}

IMUSensor::DDSHelper::DDSHelper() {
	participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (IMUMessageTypeSupport::register_type(participant, IMUMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");
}

IMUSensor::DDSHelper::~DDSHelper() {
	//delete participant; // TODO, private destructor?
}



