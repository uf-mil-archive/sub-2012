#ifndef SHOOTER_FINDER_H
#define SHOOTER_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Contours.h"

class ShooterFinder : public IFinder
{
	public:
		ShooterFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~ShooterFinder(void);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
		//EstimatePose ep;
};

#endif
