#include "HAL/HAL.h"

using namespace subjugator;
using namespace std;

HAL::HAL(DataObjectFormatter *dataobjectformat, Transport *transport, RawWire::PacketFormatterFactory packetformatfactory)
: dataobjectformat(dataobjectformat),
  rawwire(transport, packetformatfactory) {
	rawwire.configureCallbacks(boost::bind(&HAL::rawwireReadCallback, this, _1, _2), boost::bind(&HAL::rawwireErrorCallback, this, _1, _2));
}

void HAL::configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback) {
	this->readcallback = readcallback;
	this->errorcallback = errorcallback;
}

void HAL::start() {
	rawwire.start();
}

void HAL::stop() {
	rawwire.stop();
}

void HAL::write(int endnum, const DataObject &dobj) {
	rawwire.writePacket(endnum, dataobjectformat->toPacket(dobj));
}

void HAL::rawwireReadCallback(int endnum, const Packet &packet) {
	auto_ptr<DataObject> dobj(dataobjectformat->toDataObject(packet));
	if (dobj.get() && readcallback)
		readcallback(endnum, dobj);
}

void HAL::rawwireErrorCallback(int endnum, const string &msg) {
	if (errorcallback)
		errorcallback(endnum, msg);
}

