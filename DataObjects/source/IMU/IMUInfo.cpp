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

	std::cout << "Flags: " << begin[0] << std::endl;
	// There is no checksum or data header. The flags indicate whether data is good or not.
	if(begin[0])	// An error field failed. We don't push up bad data.
		return NULL;

	// get a pointer to the first byte that's data
	const boost::uint8_t *pIndex = &(*(begin + 2));

	IMUInfo *imuinfo = new IMUInfo();

	// The first field is the latest timestamp in ns
	imuinfo->timestamp = *((boost::uint64_t *)pIndex);
	pIndex += sizeof(boost::uint64_t);

	// Then the supply voltage in 14bit unsigned
	imuinfo->supplyVoltage = (*((boost::uint16_t *)pIndex))*IMUInfo::SUPPLY_CONVERSION;
	pIndex += sizeof(boost::uint16_t);

	// Then the temperature signed int
	imuinfo->temperature = (*((boost::int16_t *)pIndex)) * IMUInfo::TEMP_CONVERSION + IMUInfo::TEMP_CENTER;
	pIndex += sizeof(boost::int16_t);

	// The next 9 are gyro xyx, acc xyz, mag xyz
	for(int i = 0; i < 3; i++)
	{
		imuinfo->ang_rate(i) = (*((boost::int16_t *)pIndex)) * IMUInfo::GYRO_CONVERSION;
		pIndex += sizeof(boost::int16_t);
		imuinfo->acceleration(i) = (*((boost::int16_t *)pIndex)) * IMUInfo::ACC_CONVERSION;
		pIndex += sizeof(boost::int16_t);
		imuinfo->mag_field(i) = (*((boost::int16_t *)pIndex)) * IMUInfo::MAG_CONVERSION;
		pIndex += sizeof(boost::int16_t);
	}

	return imuinfo;
}
