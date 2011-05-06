#ifndef RAWWIRE_SHARED_H
#define RAWWIRE_SHARED_H

#include <vector>
#include <boost/cstdint.hpp>

namespace subjugator {
	typedef std::vector<boost::uint8_t> ByteVec;
	typedef ByteVec Packet; // used only to make declarations more clear
}

#endif

