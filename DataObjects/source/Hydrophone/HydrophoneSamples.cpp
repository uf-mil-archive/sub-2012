#include "DataObjects/Hydrophone/HydrophoneSamples.h"
#include <memory>

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

#define SAMPLECOUNT 256
#define BYTECOUNT (SAMPLECOUNT*2*4) // 2 bytes per sample, 4 hydrophones

HydrophoneSamples *HydrophoneSamples::parse(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	if (end - begin != BYTECOUNT)
		return NULL;

	auto_ptr<HydrophoneSamples> samples(new HydrophoneSamples());
	ByteVec::const_iterator pos = begin;

	for (int hyd=0; hyd<3; hyd++) {
		for (int sample=0; sample<SAMPLECOUNT; sample++) {
			samples->mat(sample, hyd) = (double)(pos[0] + (pos[1]<<8));
			pos += 2;
		}
	}

	return samples.release();
}

