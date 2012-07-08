#include <iostream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

#include "FinderGenerator.h"

#include "IOImages.h"

#include "BuoyFinder.h"
#include "GrapesFinder.h"
#include "PipeFinder.h"
#include "HedgeFinder.h"
#include "TubeFinder.h" 
#include "ShooterFinder.h"
#include "BinsFinder.h"
#include "GateFinder.h"
#include "WreathFinder.h"
#include <stdio.h>

using namespace std;
using namespace boost;

vector<boost::shared_ptr<IFinder> > FinderGenerator::buildFinders(const vector<string> &objectNames, const property_tree::ptree &config) {
	vector<string> buoyNames;
	vector<string> grapesNames;
	vector<string> pipeNames;
	vector<string> hedgeNames;
	vector<string> tubeNames;
	vector<string> shooterNames;
	vector<string> binsNames;
	vector<string> gateNames;
	vector<string> wreathNames;
	BOOST_FOREACH(const string &objectName, objectNames) {
		vector<string> objectPath; split(objectPath, objectName, is_any_of("/"));
		if(objectPath.size() == 0)
			throw runtime_error("empty objectName");

		if(objectPath[0] == "buoy")
			buoyNames.push_back(objectName);
		else if(objectPath[0] == "grapes")
			grapesNames.push_back(objectName);
		else if(objectPath[0] == "pipe")
			pipeNames.push_back(objectName);
		else if(objectPath[0] == "hedge")
			hedgeNames.push_back(objectName);
		else if(objectPath[0] == "tube")
			tubeNames.push_back(objectName);
		else if(objectPath[0] == "shooter")
			shooterNames.push_back(objectName);
		else if(objectPath[0] == "bins")
			binsNames.push_back(objectName);
		else if(objectPath[0] == "gate")
			gateNames.push_back(objectName);
		else if(objectPath[0] == "wreath")
			wreathNames.push_back(objectName);
	}
	
	vector<boost::shared_ptr<IFinder> > finders;
	if(buoyNames.size() > 0)
		finders.push_back(make_shared<BuoyFinder>(buoyNames, config.get_child("buoy")));
	if(grapesNames.size() > 0)
		finders.push_back(make_shared<GrapesFinder>(grapesNames, config.get_child("grapes")));
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
	if(wreathNames.size() > 0)
		finders.push_back(make_shared<WreathFinder>(wreathNames, config.get_child("wreath")));

	return finders;
}
