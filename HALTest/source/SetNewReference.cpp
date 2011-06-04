#include "HALTest/SetNewReference.h"
#include <cmath>

using namespace subjugator;
using namespace boost;
using namespace std;

SetNewReference::SetNewReference(double reference) : reference(reference) { }

void SetNewReference::appendData(Packet &packet) const {
	uint16_t referencebits = (uint16_t)(abs(reference) * 100 * pow(2, 8));
	if (reference < 0)
		referencebits |= 0x8000; // set sign bit on negative

	packet.push_back(1); // STARTPUBLISH
	packet.push_back((uint8_t)(referencebits & 0xFF));
	packet.push_back((uint8_t)(referencebits >> 8));
}

