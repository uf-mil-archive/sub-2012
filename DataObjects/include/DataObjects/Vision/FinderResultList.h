#ifndef DATAOBJECTS_VISION_FINDERRESULTLIST_H
#define DATAOBJECTS_VISION_FINDERRESULTLIST_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include "DataObjects/Vision/FinderResult.h"
#include "DataObjects/HAL/DataObject.h"

namespace subjugator {
	class FinderResultList : public DataObject {
		public:
			typedef std::vector<boost::shared_ptr<FinderResult> > ResultVec;

			FinderResultList(const ResultVec &results) : results(results) { }

			const ResultVec &getResults() const { return results; }

		private:
			 ResultVec results;
	};
}

#endif

