#include "DVL/DataObjects/DVLConfiguration.h"
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace subjugator;
using namespace boost::posix_time;
using namespace std;

DVLConfiguration::DVLConfiguration(double maxdepth) :
maxdepth(maxdepth) { }

static const char preamble[] =
	"CR0\r" // load factory settings (won't change baud rate)
	"#BJ 100 110 000\r" // enable only bottom track high res velocity and bottom track range
	"#BK2\r" // send water mass pings when bottom track pings fail
	"#BL7,36,46\r" // configure near layer and far layer to 12 and 15 feet
	"ES0\r" // 0 salinity
	"EX10010\r" // transform results to ship XYZ, allow 3 beam solutions
	"EZ10000010\r"; // configure sensor sources. Provide manual data for everything except speed of sound and temperature

Packet DVLConfiguration::makePacket() const {
	stringstream buf;
	buf << preamble;

	buf << "BX" << setw(5) << setfill('0') << (int)(maxdepth * 10) << '\r'; // configures max depth

	time_facet *lf = new time_facet("%Y/%m/%d, %H:%M:%S"); // set clock
    buf.imbue(std::locale(buf.getloc(), lf));
	buf << "TT" << second_clock::local_time() << "\r";

	buf << "CS\r"; // start pinging

	string str = buf.str();
	Packet packet(str.size());
	copy(str.begin(), str.end(), packet.begin());

	return packet;
}
