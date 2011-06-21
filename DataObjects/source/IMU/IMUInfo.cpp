#include "DataObjects/IMU/IMUInfo.h"
#include <iostream>

/*
 * This is not a good example of a dataobject converter. Since the IMU is sitting on the SPI bus,
 * this is guaranteed to be called on the same architecture(endianess) machine as the driver assembling
 * the packet. Hence, the ugly casting does work.
 */

using namespace subjugator;
using namespace std;

IMUInfo *IMUInfo::parse(ByteVec::const_iterator begin, ByteVec::const_iterator end)
{
	if (end - begin != IMUInfo::IMU_PACKET_LENGTH) // check the length
	{
		cout << "Packet invalid length" << endl;
		return NULL;
	}

	IMUInfo *imuinfo = new IMUInfo();

	// The first field is valid data flags
	imuinfo->flags = *((boost::uint16_t *)(&*((begin))));

	// Then the supply voltage in 14bit unsigned
	imuinfo->supplyVoltage = (*((boost::uint16_t *)(&*(begin+2)))) * IMUInfo::SUPPLY_CONVERSION;


	for(int i = 0; i < 3; i++)
	{
		imuinfo->ang_rate(i) = (*((boost::int16_t *)(&*(begin+4+2*i))))* IMUInfo::GYRO_CONVERSION;
		imuinfo->acceleration(i) = (*((boost::int16_t *)(&*(begin+10+2*i))))* IMUInfo::ACC_CONVERSION;
		imuinfo->mag_field(i) = (*((boost::int16_t *)(&*(begin+16+2*i))))* IMUInfo::MAG_CONVERSION;
	}

	// Then the temperature signed int
	imuinfo->temperature =  (*((boost::int16_t *)(&*(begin + 22)))) * IMUInfo::TEMP_CONVERSION + IMUInfo::TEMP_CENTER;

	imuinfo->timestamp = *((boost::int64_t *)(&*(begin + 24)));

	return imuinfo;
}



