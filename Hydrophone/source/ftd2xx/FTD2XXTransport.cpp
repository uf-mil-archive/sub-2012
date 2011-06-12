#include "Hydrophone/ftd2xx/FTD2XXTransport.h"
#include "Hydrophone/ftd2xx/FTD2XXEndpoint.h"
#include <boost/lexical_cast.hpp>
#include <vector>
#include <stdexcept>
#include <ftd2xx.h>

using namespace subjugator;
using namespace boost;
using namespace std;

FTD2XXTransport::FTD2XXTransport() { }

const string &FTD2XXTransport::getName() const {
	static const string name = "ftd2xx";
	return name;
}

Endpoint *FTD2XXTransport::makeEndpoint(const std::string &address, const ParamMap &params) {
	int devnum = findDevice(address, params);
	return new FTD2XXEndpoint(devnum);
}

int FTD2XXTransport::findDevice(const std::string &address, const ParamMap &params) {
	if (address == "any") {
		ParamMap::const_iterator i = params.find("description");
		if (i == params.end())
			throw runtime_error("Can't use 'any' address with no description set");
		return findDeviceFromDescription(i->second);
	} else {
		return lexical_cast<int>(address);
	}
}

int FTD2XXTransport::findDeviceFromDescription(const std::string &description) {
	DWORD numdevs;
	if (FT_CreateDeviceInfoList(&numdevs) != FT_OK)
		throw runtime_error("Failed to FT_CreateDeviceInfoList");

	vector<FT_DEVICE_LIST_INFO_NODE> infonodes(numdevs);
	if (FT_GetDeviceInfoList(&infonodes[0], &numdevs))
		throw runtime_error("Failed to FT_GetDeviceInfoList");

	for (int i=0; i<numdevs; i++) {
		if (infonodes[i].Description == description)
			return i;
	}

	throw runtime_error("No FTDI2XX device with description '" + description + "'");
}

