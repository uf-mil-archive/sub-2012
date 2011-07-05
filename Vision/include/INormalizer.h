#ifndef INORMALIZER_H
#define INORMALIZER_H

#include "IOImages.h"

class INormalizer
{
	public:
		virtual void norm(IOImages* ioimages)=0;
};

#endif
