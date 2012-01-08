#include "HAL/transport/FileEndpoint.h"
#include <iostream>
#include <cstring>
#include <errno.h>

using namespace subjugator;
using namespace boost;
using namespace std;

FileEndpoint::FileEndpoint(const std::string &filename, boost::asio::io_service &io) :
BaseStreamEndpoint<boost::asio::posix::stream_descriptor>(io), filename(filename) { }

void FileEndpoint::open() {
	assert(getState() == CLOSED);

	int fd = ::open(filename.c_str(), O_RDWR);
	if (fd >= 0) {
		stream.assign(fd);
		setState(OPEN);
		startAsyncSendReceive();
	} else {
		setState(ERROR, "Unable to open file " + filename + ": " + string(strerror(errno)));
	}
}
