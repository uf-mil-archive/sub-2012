#include "DDS/DDSException.h"

using namespace subjugator;
using namespace std;

static const char *retcodestr(DDS_ReturnCode_t retcode);

DDSException::DDSException(const string &msg) : runtime_error(msg) { }
DDSException::DDSException(const string &msg, DDS_ReturnCode_t retcode) : runtime_error(msg + " (" + retcodestr(retcode) + ")") { }

static const char *retcodestr(DDS_ReturnCode_t retcode) {
	switch (retcode) {
		case DDS_RETCODE_OK:                   return "ok";
		case DDS_RETCODE_ERROR:                return "error";
		case DDS_RETCODE_UNSUPPORTED:          return "unsupported";
		case DDS_RETCODE_BAD_PARAMETER:        return "bad parameter";
		case DDS_RETCODE_PRECONDITION_NOT_MET: return "precondition not met";
		case DDS_RETCODE_OUT_OF_RESOURCES:     return "out of resources";
		case DDS_RETCODE_NOT_ENABLED:          return "not enabled";
		case DDS_RETCODE_IMMUTABLE_POLICY:     return "immutable policy";
		case DDS_RETCODE_INCONSISTENT_POLICY:  return "inconsistent policy";
		case DDS_RETCODE_ALREADY_DELETED:      return "already deleted";
		case DDS_RETCODE_TIMEOUT:              return "timeout";
		case DDS_RETCODE_NO_DATA:              return "no data";
		case DDS_RETCODE_ILLEGAL_OPERATION:    return "illegal operation";
		default:                               return "unknown";
	}
}
