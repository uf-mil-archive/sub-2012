#ifndef FINDER_RESULT_H
#define FINDER_RESULT_H

#include "HAL/format/DataObject.h"

class FinderResult : public subjugator::DataObject
{
public:
	int cameraID;
	int objectID;
	double timestamp;
};

#endif
