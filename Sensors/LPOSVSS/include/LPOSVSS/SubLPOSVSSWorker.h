#ifndef _SubLPOSVSSWorker_H
#define _SubLPOSVSSWorker_H

#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include "LPOSVSS/SubNavigationComputer.h"
#include "LPOSVSS/DataObjects/DVLVelocity.h"
#include "LPOSVSS/DataObjects/IMUInfo.h"
#include "LPOSVSS/DataObjects/DepthInfo.h"
#include "LPOSVSS/DataObjects/LPOSVSSInfo.h"
#include "LPOSVSS/DataObjects/PDInfo.h"
#include <boost/ref.hpp>

namespace subjugator
{
	class LPOSVSSWorker : public Worker
	{
	public:
		LPOSVSSWorker(const WorkerConfigLoader &configloader);
		virtual ~LPOSVSSWorker();

		WorkerMailbox<DVLVelocity> dvlmailbox;
		WorkerMailbox<IMUInfo> imumailbox;
		WorkerMailbox<DepthInfo> depthmailbox;
		WorkerMailbox<PDInfo> currentmailbox;
		WorkerSignal<LPOSVSSInfo> signal;

	protected:
		virtual void enterActive();
		virtual void work(double ndt);

	private:
		boost::scoped_ptr<NavigationComputer> navComputer;
		bool useDVL;

		std::auto_ptr<DVLVelocity> dvlInfo;
		std::auto_ptr<IMUInfo> imuInfo;
		std::auto_ptr<DepthInfo> depthInfo;

		void setDepth(const boost::optional<DepthInfo>& depth);
		void setIMU(const boost::optional<IMUInfo>& imu);
		void setDVL(const boost::optional<DVLVelocity>& dvl);
		void setCurrents(const boost::optional<PDInfo>& pd);

		boost::int64_t getTimestamp(void);
	};
}

#endif // _SubLPOSVSSWorker_H

