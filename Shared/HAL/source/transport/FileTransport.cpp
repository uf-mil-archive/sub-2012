#include "HAL/transport/FileTransport.h"
#include "HAL/transport/FileEndpoint.h"
#include <vector>
#include <stdexcept>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace std;

FileTransport::FileTransport(io_service &io) : io(io) { }

const string &FileTransport::getName() const {
	static const string name = "file";
	return name;
}

Endpoint *FileTransport::makeEndpoint(const std::string &address, const ParamMap &params) {
	return new FileEndpoint(address, io);
}

