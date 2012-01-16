#ifndef DVL_DATAOBEJCTS_DVLINFO_H
#define DVL_DATAOBJECTS_DVLINFO_H

#include "HAL/format/DataObject.h"
#include <Eigen/Dense>
#include <boost/optional.hpp>
#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace subjugator {
	struct DVLStartupBanner : public DataObject { };
	
    struct DVLInfo : public DataObject {
		DVLInfo() { }
		DVLInfo(ByteVec::const_iterator begin, ByteVec::const_iterator end);
		
		boost::posix_time::ptime time;
		boost::uint16_t ensemblenum;
		double watertemp;
		
		boost::optional<Eigen::Vector3d> velocity;
		boost::optional<double> velocityerror;
		boost::optional<double> height;
		Eigen::Vector4d beamcorrelation;
    };
}

#endif
