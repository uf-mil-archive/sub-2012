#ifndef ITHRESHOLDER_RGB_ORANGE_H
#define ITHRESHOLDER_RGB_ORANGE_H

#include "IThresholder.h"

class ThresholderRGB : public IThresholder
{
public:
	ThresholderRGB(void);
	~ThresholderRGB(void);
	void thresh(IOImages* ioimages, int objectID);
	void thresh(IOImages* ioimages, boost::property_tree::ptree config);

private:
	void threshOrange(IOImages* ioimages, bool erodeDilateFlag);
	void threshRed(IOImages* ioimages, bool erodeDilateFlag);
	void threshGreen(IOImages* ioimages);
	void threshYellow(IOImages* ioimages);
	void threshBlack(IOImages* ioimages);
};

#endif
