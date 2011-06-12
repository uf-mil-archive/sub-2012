#include "Hydrophone/ftd2xx/FTD2XX.h"
#include <boost/lexical_cast.hpp>
#include <pthread.h>

using namespace subjugator;
using namespace boost;
using namespace std;

FTD2XX::FTD2XX() : handle(NULL) { }

FTD2XX::FTD2XX(int ftdnum) : handle(NULL) { open(ftdnum); }

void FTD2XX::open(int ftdnum) {
	if (FT_Open(ftdnum, &handle) != FT_OK)
		throw Error("Failed to FT_Open device number " + lexical_cast<string>(ftdnum));
}

void FTD2XX::close() {
	if (!handle)
		return;

	FT_Close(handle);
	handle = NULL;
}

size_t FTD2XX::read(uint8_t *buf, size_t bufsize) {
	DWORD dummy;
	if (FT_Read(handle, buf, 1, &dummy) != FT_OK) // blocking read for the first byte
		throw Error("Failed to FT_Read first byte");

	DWORD rxsize;
	if (FT_GetQueueStatus(handle, &rxsize) != FT_OK) // determine how many bytes follow
		throw Error("Failed to FT_GetQueueStatus");

	if (!rxsize) // no more bytes
		return 1; // just return one

	if (rxsize > bufsize-1) // don't read more bytes than the caller has room for in its buffer
		rxsize = bufsize-1;

	DWORD readcount;
	if (FT_Read(handle, buf+1, rxsize, &readcount) != FT_OK) // read the following bytes (should never block)
		throw Error("Failed to FT_Read");

	return readcount+1;

	// The FT_SetEventNotification linux implementation is fundamentally flawed! Do not use it to implement something like this method!
	// There is no way to use it without a race condition, because pthread condition variables do not act like windows event objects.
	// The race condition is that data could be received and the condition variable signaled before the application is blocking on the
	// condition variable. Condition variables must be used in conjunction with mutexes to fix this problem, but it doesn't seem like
	// FTDI has done so correctly (at least judging from their example code and API documentation)
}

size_t FTD2XX::write(const uint8_t *buf, size_t bufsize) {
	DWORD written;
	if (FT_Write(handle, const_cast<uint8_t *>(buf), bufsize, &written) != FT_OK) // FTDI const fail
		throw Error("Failed to FT_Write");
	return written;
}

