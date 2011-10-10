#include "DataObjects/Hydrophone/HydrophoneStart.h"

using namespace subjugator;
using namespace boost;

static const uint8_t startbytes_array[] = { 0xAA, 0x55, 0x17, 0x7C, 0xE7, 0x83 };
const ByteVec HydrophoneStart::startbytes(startbytes_array, startbytes_array + sizeof(startbytes_array)); // C++0x actually fixes this kind of awkwardness

