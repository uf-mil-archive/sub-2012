#include "DataObjects/IMU/IMUInfo.h"
#include <iostream>

/*
 * This is not a good example of a dataobject converter. Since the IMU is sitting on the SPI bus,
 * this is guaranteed to be called on the same architecture(endianess) machine as the driver assembling
 * the packet. Hence, the ugly casting does work.
 */

using namespace subjugator;

IMUInfo *IMUInfo::parse(ByteVec::const_iterator begin, ByteVec::const_iterator end)
{
	// Validate the length
	std::cout << "Length " << end - begin << std::endl;

	if (end - begin != IMUInfo::IMU_PACKET_LENGTH) // check the length
		return NULL;

	IMUInfo *imuinfo = new IMUInfo();

	// The first field is valid data flags
	imuinfo->flags = getU16LE(begin);

	// Then the supply voltage in 14bit unsigned
	imuinfo->supplyVoltage = getU16LE(begin+2) * IMUInfo::SUPPLY_CONVERSION;

	// The next 9 are gyro xyz, acc xyz, mag xyz
	for(int i = 0; i < 3; i++)
	{
		imuinfo->ang_rate(i) = getS16LE((begin + 4) + 2*i) * IMUInfo::GYRO_CONVERSION;
		imuinfo->acceleration(i) = getS16LE((begin + 6) + 2*i) * IMUInfo::ACC_CONVERSION;
		imuinfo->mag_field(i) = getS16LE((begin + 8) + 2*i) * IMUInfo::MAG_CONVERSION;
	}

	// Then the temperature signed int
	imuinfo->temperature =  getS16LE(begin + 22) * IMUInfo::TEMP_CONVERSION + IMUInfo::TEMP_CENTER;

	imuinfo->timestamp = getU64LE(begin + 24);

	return imuinfo;
}

boost::uint16_t IMUInfo::getU16LE(ByteVec::const_iterator pos)
{
	return (pos[0] | (pos[1]<<8));
}

boost::int16_t IMUInfo::getS16LE(ByteVec::const_iterator pos)
{
	return (pos[0] | (pos[1]<<8));
}

boost::uint64_t IMUInfo::getU64LE(ByteVec::const_iterator pos)
{
	return (boost::uint64_t)(pos[0] | (pos[1]<<8) | (pos[2]<<16) | (pos[3]<<24)) |
		   (((boost::uint64_t)pos[4]<<32) | ((boost::uint64_t)pos[5]<<40) |
		   ((boost::uint64_t)pos[6]<<48) |
		   ((boost::uint64_t)pos[7]<<56));
}


