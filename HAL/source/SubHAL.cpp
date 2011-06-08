#include "HAL/SubHAL.h"
#include "HAL/transport/TCPTransport.h"
#include "HAL/transport/UDPTransport.h"
#include "HAL/transport/SerialTransport.h"
#include "config.h"

using namespace subjugator;

SubHAL::SubHAL() {
	addTransport(new TCPTransport(iothread));
	addTransport(new UDPTransport(iothread));
	addTransport(new SerialTransport(iothread));
	loadAddressFile(embeddedAddressesPath);
}

SubHAL::~SubHAL() {
	clearTransports(); // delete transports before IOThread, since IOThread owns the io_service
}

void SubHAL::startIOThread() {
	iothread.start();
}

