#ifndef NORMALIZER_H
#define NORMALIZER_H

#include "IOImages.h"

class Normalizer {
	public:
		static void norm(IOImages* ioimages);
		static void normRGB(IOImages* ioimages);
};

#endif
