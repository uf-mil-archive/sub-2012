#include <boost/make_shared.hpp>

#include "FinderGenerator.h"

using namespace std;
using namespace boost;

FinderGenerator::FinderGenerator(void)
{
}

FinderGenerator::~FinderGenerator(void)
{
	// HOW TO MAKE SURE THAT listOfFinders is emptied and the thresholders and normalizer memory locations get cleared?
}

vector<boost::shared_ptr<IFinder> > FinderGenerator::buildFinders(vector<int> oIDs)
{
	clearFinders();	

	vector<int> buoyIDs;
	vector<int> pipeIDs;
	vector<int> hedgeIDs;
	vector<int> tubeIDs;
	vector<int> shooterIDs;
	vector<int> binsIDs;
	vector<int> shapeIDs;
	vector<int> gateIDs;

/*	if(oIDs.size() > 0)
	{
		if(oIDs[0]==MIL_OBJECTID_NO_OBJECT)
		{
			clearFinders();
			return listOfFinders;
		}
	}
*/

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		if( oIDs[i]==MIL_OBJECTID_BUOY_GREEN || oIDs[i]==MIL_OBJECTID_BUOY_RED || oIDs[i]==MIL_OBJECTID_BUOY_YELLOW )
			buoyIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_PIPE )
			pipeIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_GATE_HEDGE )
			hedgeIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_TUBE)
			tubeIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE || oIDs[i]==MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL ||
				 oIDs[i]==MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE || oIDs[i]==MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL)
			shooterIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_BIN_ALL || oIDs[i]==MIL_OBJECTID_BIN_SINGLE )
			binsIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_BIN_SHAPE)
			shapeIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_GATE_VALIDATION)
			gateIDs.push_back(oIDs[i]);
	}
	if(buoyIDs.size() > 0)
		listOfFinders.push_back(make_shared<BuoyFinder>(buoyIDs, make_shared<NormalizerRGB>(), make_shared<ThresholderRGB>()));
	if(pipeIDs.size() > 0)
		listOfFinders.push_back(make_shared<PipeFinder>(pipeIDs, make_shared<NormalizerRGB>(), make_shared<ThresholderRGB>()));
	if(hedgeIDs.size() > 0)
		listOfFinders.push_back(make_shared<HedgeFinder>(hedgeIDs, make_shared<NormalizerRGB>(), make_shared<ThresholderRGB>()));
	if(tubeIDs.size() > 0)
		listOfFinders.push_back(make_shared<TubeFinder>(tubeIDs, make_shared<NormalizerRGB>(), make_shared<ThresholderRGB>()));
	if(shooterIDs.size() > 0)
		listOfFinders.push_back(make_shared<ShooterFinder>(shooterIDs, make_shared<NormalizerRGB>(), make_shared<ThresholderRGB>()));
	if(binsIDs.size() > 0)
		listOfFinders.push_back(make_shared<BinsFinder>(binsIDs, make_shared<NormalizerRGB>(), make_shared<ThresholderRGB>()));
	if(shapeIDs.size() > 0)
		listOfFinders.push_back(make_shared<BinsFinder>(shapeIDs, make_shared<NormalizerRGB>(), make_shared<ThresholderRGB>()));
	if(gateIDs.size() > 0)
		listOfFinders.push_back(make_shared<ValidationGateFinder>(gateIDs, make_shared<NormalizerRGB>(), make_shared<ThresholderRGB>()));

	return listOfFinders;
}

void FinderGenerator::clearFinders()
{
	printf("Clearing the finder list!\n");
	// clear the finder list
	listOfFinders.clear();
}
