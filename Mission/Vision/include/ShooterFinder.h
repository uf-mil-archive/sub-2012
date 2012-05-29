#ifndef SHOOTER_FINDER_H
#define SHOOTER_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Contours.h"

class ShooterFinder : public IFinder
{
	public:
		ShooterFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
		//EstimatePose ep;
};

#endif
