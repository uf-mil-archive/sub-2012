#ifndef FINDER_RESULT_H
#define FINDER_RESULT_H

#include "HAL/format/DataObject.h"

class FinderResult : public subjugator::DataObject
{
public:
	FinderResult() : cameraID(0), objectID(0), timestamp(0) { }

	int cameraID;
	int objectID;
	double timestamp;
};

#endif
