#include "SensorLogger/DVLDDSReceiver.h"
#include "SensorLogger/IMUDDSReceiver.h"
#include "SensorLogger/DepthDDSReceiver.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include <iostream>
#include <fstream>
#include <boost/thread.hpp>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;

std::ofstream logstream;
double depth;
double humidity;
double thermistertemp;
double humiditytemp;
double temp;
double supply;
double acceleration[3];
double angular_rate[3];
double mag_field[3];
double velocity[3];
double velocityerror;

void logData() {
	logstream << second_clock::local_time().time_of_day() << ", " << velocity[0] << ", " << velocity[1] << ", " << velocity[2] << ", ";
	logstream << velocityerror << ", " << temp << ", " << supply << ", " << acceleration[0] << ", " << acceleration[1] << ", ";
	logstream << acceleration[2] << ", " << angular_rate[0] << ", " << angular_rate[1] << ", " << angular_rate[2] << ", " << mag_field[0];
	logstream <<  ", " << mag_field[1] << ", " << mag_field[2] << ", " << depth << ", " << humidity << ", " << thermistertemp << ", ";
	logstream << humiditytemp << endl;
}

void callbackDVL(const DVLMessage &message) {
	cout << "Got DVL Message!" << endl;

	cout << "Velocity: ";
	for (int i=0; i<3; i++) {
		velocity[i] = message.velocity[i];
		cout << velocity[i] << " ";
	}
	cout << endl;

	velocityerror=message.velocityerror;
	cout << "Velocity Error: " << velocityerror << endl;

	cout << endl;

	// Log Data
	logData();
}

void callbackIMU(const IMUMessage &message) {
	cout << "Got IMU Message!" << endl;

	temp = message.temp;
	supply = message.supply;

	cout << "Accels: ";
	for (int i=0; i<3; i++) {
		acceleration[i] = message.acceleration[i];
		cout << acceleration[i] << " ";
	}
	cout << endl;

	cout << "Gyros: ";
	for (int i=0; i<3; i++) {
		angular_rate[i]=message.angular_rate[i];
		cout << angular_rate[i] << " ";
	}
	cout << endl;

	cout << "Mags: ";
	for (int i=0; i<3; i++) {
		mag_field[i]=message.mag_field[i];
		cout << mag_field[i] << " ";
	}
	cout << endl;

	cout << endl;

	// Log Data
	logData();
}


void callbackDepth(const DepthMessage &message) {
	cout << "Got Depth Message!" << endl;

	depth = message.depth;
	humidity = message.humidity;
	thermistertemp = message.thermistertemp;
	humiditytemp = message.humiditytemp;

	cout << "Depth: ";
    cout << depth << " " << endl;

    cout << "Humidity: ";
    cout << humidity << " " << endl;

    cout << "Thermister Temp: ";
    cout << thermistertemp << " " << endl;

    cout << "Humidity Temp: ";
    cout << humiditytemp << " " << endl;

    cout << endl;

    // Log Data
    logData();
}

int main(int argc, char **argv) {
	// SETUP DepthDDSReceiver
	DDSDomainParticipant *participantDVL = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participantDVL)
		throw runtime_error("Failed to create DDSDomainParticipant");

	// TODO centralize all type registrations somewhere in DDSMessages
	if (DVLMessageTypeSupport::register_type(participantDVL, DVLMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	DVLDDSReceiver DVLreceiver(participantDVL, "DVL", callbackDVL);

	// SETUP IMUDDSReceiver
	DDSDomainParticipant *participantIMU = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participantIMU)
		throw runtime_error("Failed to create DDSDomainParticipant");

	// TODO centralize all type registrations somewhere in DDSMessages
	if (IMUMessageTypeSupport::register_type(participantIMU, IMUMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	IMUDDSReceiver imureceiver(participantIMU, "IMU", callbackIMU);

	// SETUP DepthDDSReceiver
	DDSDomainParticipant *participantDepth = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participantDepth)
		throw runtime_error("Failed to create DDSDomainParticipant");

	// TODO centralize all type registrations somewhere in DDSMessages
	if (DepthMessageTypeSupport::register_type(participantDepth, DepthMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	DepthDDSReceiver depthreceiver(participantDepth, "Depth", callbackDepth);

	// SETUP Logger
	char logFile[100];
	ptime time = second_clock::local_time();

	sprintf(logFile, "SensorLog_%s_%02d%02d%02d.csv", to_iso_string( day_clock::local_day() ).c_str(),
			time.time_of_day().hours(),time.time_of_day().minutes(),time.time_of_day().seconds());

	logstream.open(logFile);

	logstream << "TIME, DVLVELX, DVLVELY, DVLVELZ, DVLVELERR, IMUTEMP, IMUSUPPLY, IMUACCX, IMUACCY, IMUACCZ, IMUGYROX, IMUGYROY, ";
	logstream << "IMUGYROZ, IMUMAGX, IMUMAGY, IMUMAGZ, DEPTH, HUMIDITY, THERMISTERTEMP, HUMIDITYTEMP" << endl;

	while (true)
		this_thread::sleep(seconds(1));

	logstream.close();
}

