#include "HAL/SubHAL.h"
#include "HAL/transport/TCPTransport.h"
#include "HAL/transport/UDPTransport.h"
#include "HAL/transport/SerialTransport.h"
#include "HAL/transport/FileTransport.h"

using namespace subjugator;
using namespace boost::asio;

SubHAL::SubHAL(io_service &io) {
	addTransport(new TCPTransport(io));
	addTransport(new UDPTransport(io));
	addTransport(new SerialTransport(io));
	addTransport(new FileTransport(io));
}

