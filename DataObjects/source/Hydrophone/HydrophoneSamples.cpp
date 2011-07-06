#include "DataObjects/Hydrophone/HydrophoneSamples.h"
#include <memory>

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

#define NSEC_PER_SEC 1000000000

#define SAMPLECOUNT 256
#define BYTECOUNT (SAMPLECOUNT*2*4) // 2 bytes per sample, 4 hydrophones

HydrophoneSamples *HydrophoneSamples::parse(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	if (end - begin != BYTECOUNT)
		return NULL;

	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	auto_ptr<HydrophoneSamples> samples(new HydrophoneSamples());
	samples->timestamp = ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;

	ByteVec::const_iterator pos = begin;

	samples->mat.resize(SAMPLECOUNT,4);

	for (int sample=0; sample<SAMPLECOUNT; sample++)
	{
		samples->mat(sample, 3) = (double)(pos[0] + (pos[1]<<8));
		pos += 2;
	}
	for (int sample=0; sample<SAMPLECOUNT; sample++)
	{
		samples->mat(sample, 1) = (double)(pos[0] + (pos[1]<<8));
		pos += 2;
	}
	for (int sample=0; sample<SAMPLECOUNT; sample++)
	{
		samples->mat(sample, 2) = (double)(pos[0] + (pos[1]<<8));
		pos += 2;
	}
	for (int sample=0; sample<SAMPLECOUNT; sample++)
	{
		samples->mat(sample, 0) = (double)(pos[0] + (pos[1]<<8));
		pos += 2;
	}
	return samples.release();
}

