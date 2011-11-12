#ifndef DATAOBJECTS_MOTORDRIVER_MOTORDRIVERDATAOBJECTFORMATTER_H
#define DATAOBJECTS_MOTORDRIVER_MOTORDRIVERDATAOBJECTFORMATTER_H

#include "LibSub/DataObjects/EmbeddedDataObjectFormatter.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	typedef SimpleEmbeddedDataObjectFormatter<MotorDriverInfo> MotorDriverDataObjectFormatter;
}

#endif

