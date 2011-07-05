#ifndef SHOOTER_FINDER_H
#define SHOOTER_FINDER_H

#include "IFinder.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "Contours.h"

class ShooterFinder : public IFinder
{
	public:
		ShooterFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~ShooterFinder(void);
		vector<boost::shared_ptr<FinderResult> > find(IOImages* ioimages);

	private:
		int result;
};

#endif
