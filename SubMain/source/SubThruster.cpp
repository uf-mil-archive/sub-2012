#include "SubMain/SubThruster.h"

namespace subjugator
{
	std::ostream &operator<<(std::ostream &out, const ThrusterData &data) 
	{
		out << data.mId;

		return out;
	}

	ThrusterData::ThrusterData(std::vector<uint8_t> const &dataVector)
	{
		
	}
}
