#include "DataObjects/DVL/DVLBreak.h"
#include <algorithm>

using namespace subjugator;
using namespace std;

DVLBreak::DVLBreak() { }

Packet DVLBreak::makePacket() const {
	static const char breakstr[] = {'=', '=', '='}; // don't send a NULL after the break command

	Packet packet(sizeof(breakstr));
	copy(breakstr, breakstr + sizeof(breakstr), packet.begin());
	return packet;
}

