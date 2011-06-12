#ifndef HYDROPHONE_FTD2XX_H
#define HYDROPHONE_FTD2XX_H

#include <ftd2xx.h>
#include <boost/cstdint.hpp>
#include <stdexcept>
#include <cstdlib>

namespace subjugator {
	class FTD2XX {
		public:
			struct Error : public std::runtime_error {
				Error(const std::string &msg) : runtime_error(msg) { }
			};

			FTD2XX();
			explicit FTD2XX(int ftdnum);
			~FTD2XX() { close(); }

			void open(int ftdnum);
			void close();
			inline bool getOpen() const { return handle != NULL; }

			size_t read(uint8_t *buf, size_t bufsize); // blocks until at least one byte is read
			size_t write(const uint8_t *buf, size_t bufsize);

		private:
			FT_HANDLE handle;
	};
}

#endif

