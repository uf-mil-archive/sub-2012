#ifndef NORMALIZER_RGB_H
#define NORMALIZER_RGB_H

#include "INormalizer.h"

class NormalizerRGB : public INormalizer
{
	public:
		NormalizerRGB(void);
		~NormalizerRGB(void);
		void norm(IOImages* ioimages);
};

#endif