#ifndef DATAOBJECTS_MERGE_MERGEDATAOBJECTFORMATTER_H
#define DATAOBJECTS_MERGE_MERGEDATAOBJECTFORMATTER_H

#include "LibSub/DataObjects/EmbeddedDataObjectFormatter.h"
#include "PrimitiveDriver/DataObjects/MergeInfo.h"

namespace subjugator {
	typedef SimpleEmbeddedDataObjectFormatter<MergeInfo> MergeDataObjectFormatter;
}

#endif

