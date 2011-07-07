#include "FinderGenerator.h"

FinderGenerator::FinderGenerator(void)
{
}

FinderGenerator::~FinderGenerator(void)
{
	// HOW TO MAKE SURE THAT listOfFinders is emptied and the thresholders and normalizer memory locations get cleared?
}

vector<IFinder*> FinderGenerator::buildFinders(vector<int> oIDs)
{
	vector<int> buoyIDs;
	vector<int> pipeIDs;
	vector<int> hedgeIDs;
	vector<int> tubeIDs;
	vector<int> shooterIDs;
	vector<int> binsIDs;
	vector<int> shapeIDs;

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		if( oIDs[i]==MIL_OBJECTID_BUOY_GREEN || oIDs[i]==MIL_OBJECTID_BUOY_RED || oIDs[i]==MIL_OBJECTID_BUOY_YELLOW )
			buoyIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_PIPE )
			pipeIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_GATE_HEDGE )
			hedgeIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_TUBE || oIDs[i]==MIL_OBJECTID_GATE_VALIDATION )
			tubeIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE || oIDs[i]==MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL ||
				 oIDs[i]==MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE || oIDs[i]==MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL)
			shooterIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_BIN_ALL )
			binsIDs.push_back(oIDs[i]);
		else if( oIDs[i]==MIL_OBJECTID_BIN_SHAPE)
			shapeIDs.push_back(oIDs[i]);
	}
	if(buoyIDs.size() > 0)
		listOfFinders.push_back( new BuoyFinder( buoyIDs,new NormalizerRGB(), new ThresholderRGB() ) );
	if(pipeIDs.size() > 0)
		listOfFinders.push_back( new PipeFinder( pipeIDs, new NormalizerRGB(), new ThresholderRGB() ) );
	if(hedgeIDs.size() > 0)
		listOfFinders.push_back( new HedgeFinder( hedgeIDs, new NormalizerRGB(), new ThresholderRGB() ) );
	if(tubeIDs.size() > 0)
		listOfFinders.push_back( new TubeFinder( tubeIDs, new NormalizerRGB(), new ThresholderRGB() ) );
	if(shooterIDs.size() > 0)
		listOfFinders.push_back( new ShooterFinder( shooterIDs, new NormalizerRGB(), new ThresholderRGB() ) );
	if(binsIDs.size() > 0)
		listOfFinders.push_back( new BinsFinder( binsIDs, new NormalizerRGB(), new ThresholderRGB() ) );
	if(shapeIDs.size() > 0)
		listOfFinders.push_back( new ShapeFinder (shapeIDs, new NormalizerRGB(), new ThresholderRGB() ) );

	return listOfFinders;
}

void FinderGenerator::clearFinders()
{
	// clear the finder list
	listOfFinders.clear();
}
