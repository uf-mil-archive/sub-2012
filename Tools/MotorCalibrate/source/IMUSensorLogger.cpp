#include "MotorCalibrate/IMUSensorLogger.h"
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdexcept>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

IMUSensorLogger::IMUSensorLogger(MotorDriverController &motorcontroller)
: motorcontroller(motorcontroller),
  imureceiver(dds.participant, "IMU", bind(&IMUSensorLogger::ddsCallback, this, _1)) {
}

void IMUSensorLogger::ddsCallback(const IMUMessage &msg) {
	this->msg = msg;
	emit onNewMessage();

	if (logging) {
		const MotorDriverInfo &info = motorcontroller.getMotorInfo();
		logstream << second_clock::local_time().time_of_day() << ", " << msg.mag_field[0] << ", " << msg.mag_field[1] << ", " << msg.mag_field[2] << ", ";
		logstream << info.getReferenceInput() << ", " << info.getPresentOutput() << ", " << info.getRailVoltage() << ", " << info.getCurrent() << endl;
	}
}

void IMUSensorLogger::start(const std::string &filename) {
	logstream.open(filename.c_str());
	logging = true;
	logstream << "Time, magx, magy, magz, RefInput, PresOutput, Vrail, Current" << endl;
}

void IMUSensorLogger::stop() {
	logstream.close();
	logging = false;
}

IMUSensorLogger::DDSHelper::DDSHelper() {
	participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (IMUMessageTypeSupport::register_type(participant, IMUMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");
}

IMUSensorLogger::DDSHelper::~DDSHelper() {
	//delete participant; // TODO, private destructor?
}



