#include "DataObjects/DVL/DVLConfiguration.h"
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace subjugator;
using namespace std;

DVLConfiguration::DVLConfiguration(double maxdepth, double alignmentdeg)
: maxdepth(maxdepth), alignmentdeg(alignmentdeg) { }

static const char preamble[] =
	"CR0\r" // load factory settings (won't change baud rate)
	"#CT0\r" // turn key operation off (DVL won't start pinging automatically on startup)
	"BX00150\r" // set maximum depth to 15 meters
	"#BJ 100 110 000\r" // enable only bottom track high res velocity and bottom track range
	"ES0\r" // 0 salinity
	"EX10010\r" // transform results to ship XYZ, allow 3 beam solutions
	"EZ10000010\r"; // configure sensor sources. Provide manual data for everything except speed of sound and temperature

Packet DVLConfiguration::makePacket() const {
	stringstream buf;
	buf << preamble;

	buf << "BX" << setw(5) << setfill('0') << (int)(maxdepth * 10) << '\r'; // configures max depth

	// configure alignment
	if (alignmentdeg == 0) // don't put a + sign on zero
		buf << "EA00000\r";
	else
		buf << "EA" << setw(5) << setfill('0') << showpos << (int)(alignmentdeg * 100) << 'r'; // showpos puts a + sign for positive numbers, per DVL documentation

	buf << "CS\r"; // start pinging

	string str = buf.str();
	Packet packet(str.size());
	copy(str.begin(), str.end(), packet.begin());

	return packet;
}
