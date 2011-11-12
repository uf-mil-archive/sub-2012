#include "PrimitiveDriver/DataObjects/HeartBeatDataObjectFormatter.h"
#include "PrimitiveDriver/DataObjects/HeartBeat.h"

using namespace subjugator;
using namespace boost;

HeartBeatDataObjectFormatter::HeartBeatDataObjectFormatter(uint8_t pcaddress)
: EmbeddedDataObjectFormatter(255, pcaddress, HEARTBEAT) { }

