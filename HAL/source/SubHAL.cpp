#include "HAL/SubHAL.h"
#include "HAL/transport/TCPTransport.h"
#include "HAL/transport/UDPTransport.h"
#include "HAL/transport/SerialTransport.h"

using namespace subjugator;

SubHAL::SubHAL() {
	addTransport(new TCPTransport(iothread));
	addTransport(new UDPTransport(iothread));
	addTransport(new SerialTransport(iothread));
}

void SubHAL::startIOThread() {
	iothread.start();
}

