#ifndef ITHRESHOLDER_H
#define ITHRESHOLDER_H

#include "IOImages.h"

class IThresholder
{
	public:
		virtual void thresh(IOImages* ioimages, int objectID)=0;
};

#endif
