#include "SensorLogger/IMUDDSReceiver.h"
#include <iostream>
#include <boost/thread.hpp>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

void callback(const IMUMessage &message) {
	cout << "Got message!" << endl;

	cout << "Accels: ";
	for (int i=0; i<3; i++)
		cout << message.accel[i] << " ";
	cout << endl;

	cout << "Gyros: ";
	for (int i=0; i<3; i++)
		cout << message.gyro[i] << " ";
	cout << endl;

	cout << "Mags: ";
	for (int i=0; i<3; i++)
		cout << message.mag[i] << " ";
	cout << endl;
}

int main(int argc, char **argv) {
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	// TODO centralize all type registrations somewhere in DDSMessages
	if (IMUMessageTypeSupport::register_type(participant, IMUMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	IMUDDSReceiver imureceiver(participant, "IMU", callback);

	while (true)
		this_thread::sleep(seconds(1));
}

