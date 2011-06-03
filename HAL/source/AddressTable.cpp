#include "HAL/AddressTable.h"
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <stdexcept>
#include <sstream>
#include <algorithm>

using namespace subjugator;
using namespace boost;
using namespace std;

AddressTable::AddressTable() { }

void AddressTable::read(std::istream &in) {
	int linenum=0;
	while (true) {
		string line; // get a line
		getline(in, line);
		if (in.eof())
			break;

		static const regex cleanupreg("\\s*([^#]*).*"); // strip any indentation and comments
		smatch match;
		regex_match(line, match, cleanupreg);
		string cleanline = match[1];

		if (cleanline.size() == 0)
			continue;

		static const regex entryreg("(\\d+)\\s*(\\w+)\\s*(\\S+)\\s*(.*)"); // parse the protocol, protocol address, and parameters

		if (!regex_match(cleanline, match, entryreg))
			throw runtime_error("Parse error on line " + boost::lexical_cast<string>(linenum));

		int addr = boost::lexical_cast<int>(match[1]);
		Entry entry;
		entry.protocol = match[2];
		entry.protoaddress = match[3];
		const string &paramstring = match[4];

		if (paramstring.size() > 0) {
			static const regex paramreg("(\\w+)\\s*=\\s*(\\w+)\\s*");

			sregex_iterator regi(paramstring.begin(), paramstring.end(), paramreg);
			for (; regi != sregex_iterator(); ++regi) {
				const smatch &match = *regi;
				entry.params.insert(make_pair(match[1], match[2]));
			}
		}

		entrymap.insert(make_pair(addr, entry));
	}
}

void AddressTable::addEntry(int addr, const Entry &entry) {
	entrymap.insert(make_pair(addr, entry));
}

bool AddressTable::hasEntry(int addr) const {
	return entrymap.find(addr) != entrymap.end();
}

const AddressTable::Entry &AddressTable::getEntry(int addr) const {
	EntryMap::const_iterator i = entrymap.find(addr);
	if (i != entrymap.end())
		return i->second;
	else
		throw runtime_error("AddressTable::getEntry called on non-existent entry " + boost::lexical_cast<string>(addr));
}

