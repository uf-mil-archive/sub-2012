#ifndef THRESHOLDER_H
#define THRESHOLDER_H

#include <boost/property_tree/ptree.hpp>

#include "IOImages.h"

class Thresholder {
	public:
		static void threshBuoys(IOImages *ioimages);
		static void threshConfig(IOImages* ioimages, boost::property_tree::ptree config);
		static void threshOrange(IOImages* ioimages);
		static void threshRed(IOImages* ioimages);
		static void threshShooterRed(IOImages* ioimages);
		static void threshGreen(IOImages* ioimages);
		static void threshYellow(IOImages* ioimages);
		static void threshBlue(IOImages* ioimages);
		static void threshBlack(IOImages* ioimages);
};

#endif
