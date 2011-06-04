#include "HAL/SubHAL.h"
#include "HAL/TCPTransport.h"
#include "HAL/UDPTransport.h"
#include "HAL/SerialTransport.h"

using namespace subjugator;

SubHAL::SubHAL() {
	addTransport(new TCPTransport(iothread));
	addTransport(new UDPTransport(iothread));
	addTransport(new SerialTransport(iothread));

	iothread.start();
}

