#ifndef ITHRESHOLDER_H
#define ITHRESHOLDER_H

#include <boost/property_tree/ptree.hpp>

#include "IOImages.h"

class IThresholder
{
	public:
		virtual void thresh(IOImages* ioimages, int objectID)=0;
		virtual void thresh(IOImages* ioimages, boost::property_tree::ptree config)=0;
};

#endif
