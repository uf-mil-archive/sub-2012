#include "DVL/DataObjects/DVLInfo.h"
#include <set>

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;

static uint16_t parseDataType(DVLInfo &info, ByteVec::const_iterator begin, ByteVec::const_iterator end);
static uint16_t getU16LE(ByteVec::const_iterator i);
static int16_t getS16LE(ByteVec::const_iterator i);
static int32_t getS32LE(ByteVec::const_iterator i);

static const uint16_t VARIABLE_LEADER = 0x0080;
static const uint16_t HIGH_RESOLUTION_BOTTOM_TRACK = 0x5803;
static const uint16_t BOTTOM_TRACK = 0x0600;
static const uint16_t BOTTOM_TRACK_RANGE = 0x5804;

DVLInfo::DVLInfo(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
    set<uint16_t> headers;
    
    int length = getU16LE(begin+2); // extract the number of bytes
    int datatypes = begin[5]; // extract the number of data type
    
    for (int i=0; i<datatypes; i++) { // for each datatype
	int startoffset = getU16LE(begin + 6 + i*2); // extract its starting offset
	int nextoffset;
	if (i != datatypes-1) // if we're not on the last one
	    nextoffset = getU16LE(begin + 6 + (i+1)*2); // find the next offset
	else
	    nextoffset = length - 2; // otherwise treat the next offset as the position of the end of the packet, sans a reserved word

	uint16_t header = parseDataType(*this, begin + startoffset, begin + nextoffset); // parse the data type
	if (header != 0) // if we recognized the type's header
	    headers.insert(header); // add the header to our set
    }
    
    if (!headers.count(VARIABLE_LEADER))
	throw runtime_error("Ensemble missing variable leader");
    if (!headers.count(HIGH_RESOLUTION_BOTTOM_TRACK))
	throw runtime_error("Ensemble missing high resolution bottom track");
    if (!headers.count(BOTTOM_TRACK))
	throw runtime_error("Ensemble missing bottom track");
    if (!headers.count(BOTTOM_TRACK_RANGE))
	throw runtime_error("Ensemble missing bottom track range");
}

static const boost::int32_t BADVEL = -3276801;

static uint16_t parseDataType(DVLInfo &info, ByteVec::const_iterator begin, ByteVec::const_iterator end) {
    uint16_t header = getU16LE(begin);
    int length = end - begin;
    
    switch (header) {
	case VARIABLE_LEADER: {
	    if (length != 60) {
		stringstream buf;
		buf << "Wrong length on variable leader: " << length;
		throw runtime_error(buf.str());
	    }
		
	    date d(2000 + begin[4], begin[5], begin[6]);
	    time_duration t = hours(begin[7]) + minutes(begin[8]) + seconds(begin[9]) + milliseconds(begin[10]*10);
	    info.time = ptime(d, t);
	    info.ensemblenum = getU16LE(begin+2);
	    info.watertemp = getS16LE(begin + 26)*.01;
	    break;
	}
	
	case HIGH_RESOLUTION_BOTTOM_TRACK: {
	    if (length != 70) {
		stringstream buf;
		buf << "Wrong length on high resolution bottom track: " << length;
		throw runtime_error(buf.str());
	    }
		
	    Vector3d vel;
	    bool good = true;
	    for (int i=0; i<3; i++) { // grab X Y Z velocity
		int32_t val = getS32LE(begin+2+4*i);
		if (val != BADVEL) {
		    vel(i) = val / 100000.0;
		} else {
		    good = false;
		    break;
		}
	    }
	    if (good)
		info.velocity = vel;
	    
	    int32_t val = getS32LE(begin+14);
	    if (val != BADVEL)
		info.velocityerror = val / 1000000.0;
	    break;
	}
	
	case BOTTOM_TRACK: {
	    if (length != 81) {
		stringstream buf;
		buf << "Wrong length on bottom track: " << length;
		throw runtime_error(buf.str());
	    }
		
	    for (int i=0; i<4; i++)
		info.beamcorrelation(i) = begin[i+32] / 255.0;
	    break;
	}
	
	case BOTTOM_TRACK_RANGE: {
	    if (length != 39) {
		stringstream buf;
		buf << "Wrong length on bottom track range: " << length;
		throw runtime_error(buf.str());
	    }
	    
	    int32_t range = getS32LE(begin+10);
	    if (range != 0)
		info.height = range / 10000.0;
	    break;
	}
	
	default:
	    return 0;
    }
    
    return header;
}

static uint16_t getU16LE(ByteVec::const_iterator i) {
    return i[0] | (i[1]<<8);
}

static int16_t getS16LE(ByteVec::const_iterator i) {
    return i[0] | (i[1]<<8);
}

static int32_t getS32LE(ByteVec::const_iterator i) {
    return i[0] | (i[1]<<8) | (i[2]<<16) | (i[3]<<24);
}
