#ifndef IFINDER_H
#define IFINDER_H

#include <utility>
#include <vector>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

#include <opencv/cv.h>

#include "IOImages.h"

class IFinder
{
	public:
		IFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) {
			this->objectNames = objectNames;
			this->config = config;
		}
		std::vector<std::string> objectNames;
		virtual std::vector<boost::property_tree::ptree> find(IOImages* ioimages) = 0;
	protected:
		boost::property_tree::ptree config;
		boost::property_tree::ptree Point_to_ptree(const cv::Point& p, const cv::Mat& img) {
			boost::property_tree::ptree result;
			result.push_back(std::make_pair("", boost::lexical_cast<std::string>(2*(p.x+0.5)/img.size().width  - 1)));
			result.push_back(std::make_pair("", boost::lexical_cast<std::string>(1 - 2*(p.y+0.5)/img.size().height)));
			return result;
		}
};

#endif
