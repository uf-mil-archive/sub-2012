#ifndef SIMSUB_H
#define SIMSUB_H

#include "SuckySim/SimObject.h"
#include "SuckySim/SimCamera.h"
#include "SuckySim/SimHelpers.h"
#include <Eigen/Dense>
#include "SubMain/Workers/TrackingController/TrajectoryGenerator.h"
#include "SubMain/Workers/MissionPlanner/SubMissionCamera.h"
#include "SubMain/Workers/MissionPlanner/SubMissionEnums.h"
#include <boost/asio/deadline_timer.hpp>

using namespace Eigen;

namespace subjugator
{
	class SimSub : public SimObject
	{
	public:
		SimSub(std::string name, int objectID, QColor color);

		void setForwardSearch(ObjectIDs::ObjectIDCode obj) { forwardSearchObject = obj; }
		void setDownSearch(ObjectIDs::ObjectIDCode obj) { downSearchObject = obj; }

	protected:
		virtual void Draw(QPainter* painter, double pPerM);
		virtual bool PointIsInside(Vector2d p, double mPerPix);

	private:
			TrajectoryGenerator trajGen;
			double length;
			double width;
			double podLength;
			double podWidth;

			Vector3d velocity_NED;

			std::auto_ptr<SimCamera> forwardCamera;
			std::auto_ptr<SimCamera> downCamera;

			ObjectIDs::ObjectIDCode forwardSearchObject;
			ObjectIDs::ObjectIDCode downSearchObject;

			void UpdateOthers();
	};
}

#endif /* SIMSUB_H */
