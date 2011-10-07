#include "HAL/SubHAL.h"
#include "HAL/transport/TCPTransport.h"
#include "HAL/transport/UDPTransport.h"
#include "HAL/transport/SerialTransport.h"
#include "HAL/transport/FileTransport.h"
#include "config.h"

using namespace subjugator;
using namespace boost::asio;

SubHAL::SubHAL() {
	io_service &ioservice = iothread.getIOService();
	addTransport(new TCPTransport(ioservice));
	addTransport(new UDPTransport(ioservice));
	addTransport(new SerialTransport(ioservice));
	addTransport(new FileTransport());
	loadAddressFile(embeddedAddressesPath);
}

SubHAL::~SubHAL() {
	iothread.stop(); // stop the IOThread before clearing the transports, since it may manage to get a callback in on a non-existent Transport
	clearTransports(); // delete transports before IOThread, since IOThread owns the io_service
}

void SubHAL::startIOThread() {
	iothread.start();
}

