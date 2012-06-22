#ifndef PRIMITIVEDRIVER_DATAOBJECTS_ACTUATORDATAOBJECTS
#define PRIMITIVEDRIVER_DATAOBJECTS_ACTUATORDATAOBJECT

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <bitset>

namespace subjugator {
	struct Ping : public DataObject {
		static const boost::uint8_t CMD_MASK = 0xFF;
		static const boost::uint8_t CMD_NUMBER = 0x40;
	};

	struct SetValves : public DataObject {
		typedef std::bitset<6> Bits;

		Bits bits;

		SetValves() { }
		SetValves(Bits bits) : bits(bits) { }

		static const boost::uint8_t CMD_MASK = 0xC0;
		static const boost::uint8_t CMD_NUMBER = 0x80;
	};

	struct ReadSwitches : public DataObject {
		static const boost::uint8_t CMD_NUMBER = 0x00;
	};

	struct SwitchValues : public DataObject {
		bool in1;
		bool in2;

		SwitchValues() { }
		SwitchValues(bool in1, bool in2) : in1(in1), in2(in2) { }
		SwitchValues(boost::uint8_t cmd) : in1((cmd&0x01) != 0), in2((cmd&0x02) != 0) { }

		static const boost::uint8_t CMD_MASK = 0xFC;
		static const boost::uint8_t CMD_NUMBER = 0x00;
	};
}

#endif
