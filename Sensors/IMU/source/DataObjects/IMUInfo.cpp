#include "IMU/DataObjects/IMUInfo.h"

/*
 * This is not a good example of a dataobject converter. Since the IMU is sitting on the SPI bus,
 * this is guaranteed to be called on the same architecture(endianess) machine as the driver assembling
 * the packet. Hence, the ugly casting does work.
 */

using namespace subjugator;
using namespace boost;
using namespace std;

static const double SUPPLY_CONVERSION = 0.00242;      // V

static const double TEMP_CONVERSION = 0.14;	          // degC / bit
static const double TEMP_CENTER = 25.0;               // Temps are centered around 25.0 degC

static const double GYRO_CONVERSION = 0.000872664626; // rad/s
static const double ACC_CONVERSION = 0.0033;          // g's
static const double MAG_CONVERSION = 0.0005;          // gauss



static uint16_t getu16(ByteVec::const_iterator i) { return reinterpret_cast<const boost::uint16_t &>(*i); }
static int16_t get16(ByteVec::const_iterator i) { return reinterpret_cast<const boost::int16_t &>(*i); }
static int64_t get64(ByteVec::const_iterator i) { return reinterpret_cast<const boost::int64_t &>(*i); }

bool IMUInfo::parse(IMUInfo &info, ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	if (end - begin != PACKET_LENGTH)
		return false;

	// The first field is valid data flags
	info.flags = getu16(begin);

	// Then the supply voltage in 14bit unsigned
	info.supplyVoltage = getu16(begin + 2) * SUPPLY_CONVERSION;

	for(int i = 0; i < 3; i++) {
		info.ang_rate(i) = get16(begin + 4+2*i) * GYRO_CONVERSION;
		info.acceleration(i) = get16(begin + 10+2*i) * ACC_CONVERSION;
		info.mag_field(i) = get16(begin + 16+2*i) * MAG_CONVERSION;
	}

	// Then the temperature signed int
	info.temperature = get16(begin + 22) * TEMP_CONVERSION + TEMP_CENTER;
	
	info.timestamp = get64(begin + 24);

	return true;
}



