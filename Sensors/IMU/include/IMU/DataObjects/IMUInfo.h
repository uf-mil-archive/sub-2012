#ifndef IMU_DATAOBEJCTS_IMUINFO_H
#define IMU_DATAOBEJCTS_IMUINFO_H

#include <boost/cstdint.hpp>
#include "HAL/format/DataObject.h"
#include <Eigen/Dense>
#include <math.h>

/*
 * This is not a good example of a dataobject converter. Since the IMU is sitting on the SPI bus,
 * this is guaranteed to be called on the same architecture(endianess) machine as the driver assembling
 * the packet. Hence, the ugly casting does work.
 */
namespace subjugator {
	struct IMUInfo : public DataObject {
		static bool parse(IMUInfo &info, ByteVec::const_iterator begin, ByteVec::const_iterator end);

		static const int PACKET_LENGTH = 32;

		boost::uint16_t flags;
		double supplyVoltage;
		double temperature;
		boost::uint64_t timestamp;
		Eigen::Vector3d acceleration;
		Eigen::Vector3d ang_rate;
		Eigen::Vector3d mag_field;
	};
}

#endif
