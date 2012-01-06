#ifndef HAL_ADDRESSTABLE_H
#define HAL_ADDRESSTABLE_H

#include <string>
#include <map>
#include <istream>

namespace subjugator {
	class AddressTable {
		public:
			struct Entry {
				std::string protocol;
				std::string protoaddress;
				std::map<std::string, std::string> params;
			};

			AddressTable();

			void read(std::istream &in);

			void addEntry(int addr, const Entry &entry);
			bool hasEntry(int addr) const;
			const Entry &getEntry(int addr) const;

		private:
			typedef std::map<int, Entry> EntryMap;
			EntryMap entrymap;
	};
}

#endif

