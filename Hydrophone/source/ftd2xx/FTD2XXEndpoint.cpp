#include "Hydrophone/ftd2xx/FTD2XXEndpoint.h"
#include <boost/lexical_cast.hpp>
#include <ftd2xx.h>

using namespace subjugator;
using namespace boost;
using namespace std;

FTD2XXEndpoint::FTD2XXEndpoint(int devnum) : devnum(devnum) { }

void FTD2XXEndpoint::open() {
	assert(getState() == CLOSED);

	try {
		ftd.open(devnum);
		readthread = thread(&FTD2XXEndpoint::readthread_run, this);
		writethread = thread(&FTD2XXEndpoint::writethread_run, this);
		setState(OPEN);
	} catch (FTD2XX::Error &err) {
		setState(ERROR, err.what());
	}
}

void FTD2XXEndpoint::close() {
	ftd.close();
	readthread.join();
	writethread.join();
	setState(CLOSED);
}

void FTD2XXEndpoint::write(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	lock_guard<mutex> lock(writemutex);
	writebuf.insert(writebuf.end(), begin, end);
	writecond.notify_one();
}

void FTD2XXEndpoint::readthread_run() {
	ByteVec recvbuf(4096);

	try {
		while (true) {
			size_t got = ftd.read(&recvbuf[0], recvbuf.size());
			callReadCallback(recvbuf.begin(), recvbuf.begin() + got);
		}
	} catch (FTD2XX::Error &err) {
		if (ftd.getOpen())
			setState(ERROR, "Read thread terminated: " + string(err.what()));
	}
}

void FTD2XXEndpoint::writethread_run() {
	ByteVec outbuf;

	try {
		while (true) {
			unique_lock<mutex> lock(writemutex);
			while (writebuf.size() == 0)
				writecond.wait(lock);

			swap(writebuf, outbuf); // zero-copy is really easy to achieve with swap() on vectors
			lock.unlock();

			size_t wrote=0;
			while (wrote < outbuf.size())
				wrote += ftd.write(&outbuf[wrote], outbuf.size() - wrote);
			outbuf.clear();
		}
	} catch (FTD2XX::Error &err) {
		if (ftd.getOpen())
			setState(ERROR, "Write thread terminated: " + string(err.what()));
	}
}

