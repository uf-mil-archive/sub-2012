#include "HAL/SubHAL.h"
#include "HAL/format/SPIPacketFormatter.h"
#include "DataObjects/IMU/IMUDataObjectFormatter.h"
#include "DataObjects/IMU/IMUInfo.h"

#include <ndds/ndds_cpp.h>

#include "DDSListeners/IMUDDSListener.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

IMUDDSListener *listener;

void stateChangeCallback()
{

}

void receiveCallback(auto_ptr<DataObject> &dobj)
{
	static int divisor = 0;

	divisor = (divisor + 1) % 1000;

	if(divisor)
		return;

	if (IMUInfo *info = dynamic_cast<IMUInfo *>(dobj.get()))
	{
		listener->Publish(info);


		/*cout << "Flags: " << info->getFlags() << endl;
		cout << "Timestamp: " << info->getTimestamp() << endl;
		cout << "Supply Voltage: " << info->getSupplyVoltage() << endl;
		cout << "Acceleration: " << info->getAcceleration() << endl;
		cout << "Gyro: " << info->getAngularRate() << endl;
		cout << "Magnetometer: " << info->getMagneticField() << endl;
		cout << "Temperature: " << info->getTemperature() << endl;*/
	}
}

int main(int argc, char **argv)
{
	SubHAL hal;

	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (IMUMessageTypeSupport::register_type(participant, IMUMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	listener = new IMUDDSListener(participant);

	scoped_ptr<DataObjectEndpoint> endpoint(hal.openDataObjectEndpoint(25, new IMUDataObjectFormatter(), new SPIPacketFormatter(32)));

	endpoint->configureCallbacks(receiveCallback, stateChangeCallback);
	endpoint->open();

	if(endpoint->getState() == Endpoint::ERROR)
	{
		cout << endpoint->getErrorMessage() << endl;
		return 1;
	}

	hal.startIOThread();

	while (true)
		this_thread::sleep(seconds(1));

	delete(listener);
}

