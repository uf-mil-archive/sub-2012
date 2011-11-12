#include "PrimitiveDriver/DataObjects/SetReference.h"
#include "LibSub/Math/FixedPoint.h"
#include <cmath>

using namespace subjugator;
using namespace boost;
using namespace std;

SetReference::SetReference(double reference) : reference(reference) { }

void SetReference::appendDataPacket(Packet &packet) const {
	uint16_t referencebits = toFixedPoint<8, uint16_t>(-reference); // negative since motors are wired incorrectly
	packet.push_back((uint8_t)(referencebits & 0xFF));
	packet.push_back((uint8_t)(referencebits >> 8));
}

