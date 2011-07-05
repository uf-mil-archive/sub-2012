#include "DDSListeners/VisionDDSListener.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/FinderResult3D.h"

using namespace subjugator;

VisionDDSListener::VisionDDSListener(Worker &worker, DDSDomainParticipant *part)
: finder2dsender(part, "Finder2D"),
  finder3dsender(part, "Finder3D") {
	connectWorker(worker);
}

void VisionDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	if (FinderResult2D *result2d = dynamic_cast<FinderResult2D *>(dobj.get())) {
		Finder2DMessage *msg = Finder2DMessageTypeSupport::create_data();
		msg->u = result2d->u;
		msg->v = result2d->v;
		msg->scale = result2d->scale;
		msg->angle = result2d->angle;
		finder2dsender.Send(*msg);
		Finder2DMessageTypeSupport::delete_data(msg);
	} else if (FinderResult3D *result3d = dynamic_cast<FinderResult3D *>(dobj.get())) {
		Finder3DMessage *msg = Finder3DMessageTypeSupport::create_data();
		msg->x = result3d->x;
		msg->y = result3d->y;
		msg->z = result3d->z;
		msg->ang1 = result3d->ang1;
		msg->ang2 = result3d->ang2;
		msg->ang3 = result3d->ang3;
	}
}
