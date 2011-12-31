/**
\headerfile HAL/shared.h
\file HAL/shared.h

\brief shared definitions used by the HAL
*/

#ifndef RAWWIRE_SHARED_H
#define RAWWIRE_SHARED_H

#include <vector>
#include <boost/cstdint.hpp>

/** \brief namespace containing all SubjuGator code */
namespace subjugator {
	/** \brief std::vector of uint8_t.

	Type used to pass around generic chunks of bytes throughout the HAL.
	*/

	typedef std::vector<boost::uint8_t> ByteVec;

	/** \brief Semantic typedef of ByteVec

	Packet is equivalent to a subjugator::ByteVec, its only purpose is to make definitions
	involving both raw bytes and complete packets clearer.
	*/
	typedef ByteVec Packet;
}

#endif

