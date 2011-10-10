/*
 * HydrophoneInfo.h
 *
 *  Created on: Jul 3, 2011
 *      Author: gbdash
 */

#ifndef HYDROPHONEINFO_H_
#define HYDROPHONEINFO_H_

#include <boost/cstdint.hpp>
#include "HAL/format/DataObject.h"

namespace subjugator {
	class HydrophoneInfo : public DataObject {
		public:
			HydrophoneInfo(boost::uint64_t timestamp, double distance, double heading, double declination, double pingfrequency, bool valid)
			: timestamp(timestamp), distance(distance), heading(heading), declination(declination), pingfrequency(pingfrequency), valid(valid){}

			boost::uint64_t getTimestamp() const { return timestamp; }
			double getDistance() const { return distance; }
			double getHeading() const { return heading; }
			double getDeclination() const { return declination; }
			double getPingfrequency() const { return pingfrequency; }
			bool isValid() const { return valid; }

		private:
			boost::uint64_t timestamp;
			double distance;
			double heading;
			double declination;
			double pingfrequency;
			bool valid;
	};
}

#endif /* HYDROPHONEINFO_H_ */
