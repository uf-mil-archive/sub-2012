#include "HAL/HAL.h"

using namespace subjugator;
using namespace std;

HAL::HAL(DataObjectFormatter *dataobjectformat, Transport *transport, RawWire::PacketFormatterFactory packetformatfactory)
: dataobjectformat(dataobjectformat),
  rawwire(transport, packetformatfactory) {
	rawwire.configureCallbacks(boost::bind(&HAL::transportReadCallback, this, _1, _2), boost::bind(&HAL::transportErrorCallback, this, _1, _2));
}

void HAL::configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback) {
	this->readcallback = readcallback;
	this->errorcallback = errorcallback;
}

void HAL::write(int endnum, const DataObject &dataobject) {
	rawwire.writePacket(endnum, dataobject.toPacket());
}

void HAL::transportReadCallback(int endnum, const ByteVec &bytes) {
	auto_ptr<DataObject> dobj(dataobjectformat->toDataObject(bytes));
	if (dobj.get() && readcallback)
		readcallback(endnum, dobj);
}

void HAL::transportErrorCallback(int endnum, const string &msg) {
	if (errorcallback)
		errorcallback(endnum, msg);
}

