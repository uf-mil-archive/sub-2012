#ifndef DATAOBJECTS_DATAOBJECTVEC_H
#define DATAOBJECTS_DATAOBJECTVEC_H

#include "HAL/format/DataObject.h"
#include <boost/shared_ptr.hpp>

namespace subjugator {
	struct DataObjectVec : public DataObject {
		typedef std::vector<boost::shared_ptr<DataObject> > Vec;
		DataObjectVec() { }
		DataObjectVec(const Vec &vec) : vec(vec) { }

		Vec vec;
	};
}

#endif

