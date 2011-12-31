#include "HAL/SubHAL.h"
#include "HAL/transport/TCPTransport.h"
#include "HAL/transport/UDPTransport.h"
#include "HAL/transport/SerialTransport.h"
#include "HAL/transport/FileTransport.h"

using namespace subjugator;
using namespace boost::asio;

SubHAL::SubHAL(io_service &ioservice) {
	addTransport(new TCPTransport(ioservice));
	addTransport(new UDPTransport(ioservice));
	addTransport(new SerialTransport(ioservice));
	addTransport(new FileTransport(ioservice));
}

