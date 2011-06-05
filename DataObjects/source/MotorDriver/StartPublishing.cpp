#include "DataObjects/MotorDriver/StartPublishing.h"

using namespace subjugator;
using namespace boost;

StartPublishing::StartPublishing(uint8_t rate) : rate(rate) { }

void StartPublishing::appendDataPacket(Packet &packet) const {
	packet.push_back(1);
	packet.push_back(rate);
}

