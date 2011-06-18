#include "HAL/SubHAL.h"
#include "DataObjects/IMU/IMUDataObjectFormatter.h"
#include "DataObjects/IMU/IMUInfo.h"
#include "HAL/format/SPIPacketFormatter.h"
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

void stateChangeCallback()
{

}

void receiveCallback(auto_ptr<DataObject> &dobj)
{
	static int divisor = 0;

	divisor = (divisor + 1) % 100;

	if(divisor)
		return;

	if (IMUInfo *info = dynamic_cast<IMUInfo *>(dobj.get()))
	{
		cout << "Flags: " << info->getFlags() << endl;
		cout << "Timestamp: " << info->getTimestamp() << endl;
		cout << "Supply Voltage: " << info->getSupplyVoltage() << endl;
		cout << "Acceleration: " << info->getAcceleration() << endl;
		cout << "Gyro: " << info->getAngularRate() << endl;
		cout << "Magnetometer: " << info->getMagneticField() << endl;
		cout << "Temperature: " << info->getTemperature() << endl;
	}
}

int main(int argc, char **argv)
{
	cout<< "Running\n";

	SubHAL hal;

	scoped_ptr<DataObjectEndpoint> endpoint(hal.openDataObjectEndpoint(25, new IMUDataObjectFormatter(), new SPIPacketFormatter(32)));

	cout << "Past endpoint constructor.\n";

	endpoint->configureCallbacks(receiveCallback, stateChangeCallback);
	endpoint->open();

	if(endpoint->getState() == Endpoint::ERROR)
	{
		cout << endpoint->getErrorMessage() << endl;
		return 1;
	}

	hal.startIOThread();

	cout << "Go" << endl;

	while (true)
		this_thread::sleep(seconds(1));
}

