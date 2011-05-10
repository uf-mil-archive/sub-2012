// Any literal strings go here. That way they are easy to locate and change

#ifndef _SubTranslations_H__
#define _SubTranslations_H__

#include <string>

namespace subjugator
{
	const std::string STATE_STARTUP_STRING = "STARTUP";
	const std::string STATE_INITIALIZE_STRING = "INITIALIZE";
	const std::string STATE_READY_STRING = "READY";
	const std::string STATE_STANDBY_STRING = "STANDBY";
	const std::string STATE_EMERGENCY_STRING = "EMERGENCY";
	const std::string STATE_FAIL_STRING = "FAIL";
	const std::string STATE_SHUTDOWN_STRING = "SHUTDOWN";
	const std::string STATE_UNKNOWN_STRING = "UNKNOWN";
	const std::string STATE_ALL_STRING = "ALL";

	const std::string PSTATE_SEARCH_STRING = "SEARCH";
	const std::string PSTATE_INMSG_STRING = "INMSG";
	const std::string PSTATE_ESCAPED_STRING = "ESCAPED";
}


#endif // _SubTranslations_H__

