#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

#include "FinderGenerator.h"

#include "IOImages.h"

#include "BuoyFinder.h"
#include "PipeFinder.h"
#include "HedgeFinder.h"
#include "TubeFinder.h" 
#include "ShooterFinder.h"
#include "BinsFinder.h"
#include "GateFinder.h"
#include <stdio.h>

using namespace std;
using namespace boost;

vector<boost::shared_ptr<IFinder> > FinderGenerator::buildFinders(vector<string> objectNames, const property_tree::ptree& config) {
	vector<string> buoyNames;
	vector<string> pipeNames;
	vector<string> hedgeNames;
	vector<string> tubeNames;
	vector<string> shooterNames;
	vector<string> binsNames;
	vector<string> gateNames;
	BOOST_FOREACH(string &objectName, objectNames) {
		if(objectName == "buoy/green" || objectName=="buoy/red" || objectName == "buoy/yellow")
			buoyNames.push_back(objectName);
		else if(objectName == "pipe")
			pipeNames.push_back(objectName);
		else if(objectName == "hedge")
			hedgeNames.push_back(objectName);
		else if(objectName == "tube")
			tubeNames.push_back(objectName);
		else if(objectName == "shooter/blue/large" || objectName == "shooter/blue/small" ||
				objectName == "shooter/red/large" || objectName== "shooter/red/small")
			shooterNames.push_back(objectName);
		else if(objectName == "bins/all" || objectName == "bins/single" || objectName == "bins/shape")
			binsNames.push_back(objectName);
		else if(objectName == "gate")
			gateNames.push_back(objectName);
	}
	
	vector<boost::shared_ptr<IFinder> > finders;
	if(buoyNames.size() > 0)
		finders.push_back(make_shared<BuoyFinder>(buoyNames, config.get_child("buoy")));
	if(pipeNames.size() > 0)
		finders.push_back(make_shared<PipeFinder>(pipeNames, config.get_child("pipe")));
	if(hedgeNames.size() > 0)
		finders.push_back(make_shared<HedgeFinder>(hedgeNames, config.get_child("hedge")));
	if(tubeNames.size() > 0)
		finders.push_back(make_shared<TubeFinder>(tubeNames, config.get_child("tube")));
	if(shooterNames.size() > 0)
		finders.push_back(make_shared<ShooterFinder>(shooterNames, config.get_child("shooter")));
	if(binsNames.size() > 0)
		finders.push_back(make_shared<BinsFinder>(binsNames, config.get_child("bins")));
	if(gateNames.size() > 0)
		finders.push_back(make_shared<GateFinder>(gateNames, config.get_child("gate")));

	return finders;
}
