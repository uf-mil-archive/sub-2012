#include "DataObjects/Hydrophone/HydrophoneStart.h"

using namespace subjugator;
using namespace boost;

static const uint8_t startbytes_array[] = { 0xAA, 0x55, 0xFF, 0x7F, 0x01, 0x80 };
const ByteVec HydrophoneStart::startbytes(startbytes_array, startbytes_array + sizeof(startbytes_array)); // C++0x actually fixes this kind of awkwardness

