#ifndef _SubStateObject_H__
#define _SubStateObject_H__

#include "SubMain/SubPrerequisites.h"

namespace subjugator
{
 	class StateObject
	{
	public:
		StateObject(std::string name, boost::function<void(void)> callback)
			: Name(name)
		{
			// A null or empty function pointer is illegal
			assert(callback && !callback.empty());

			Execute = callback;
		}

		std::string Name;
		boost::function<void(void)> Execute;
	};
}

#endif // _SubStateObject_H__

