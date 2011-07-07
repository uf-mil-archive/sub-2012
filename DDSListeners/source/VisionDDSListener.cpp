#include "DDSListeners/VisionDDSListener.h"
#include "DataObjects/DataObjectVec.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/FinderResult3D.h"
#include "DDSMessages/Finder2DMessage.h"
#include "DDSMessages/Finder2DMessageSupport.h"
#include "DDSMessages/Finder3DMessage.h"
#include "DDSMessages/Finder3DMessageSupport.h"

using namespace subjugator;
using namespace boost;

VisionDDSListener::VisionDDSListener(Worker &worker, DDSDomainParticipant *part)
: finderlistsender(part, "Vision") {
	connectWorker(worker);
}

void VisionDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	if (DataObjectVec *dobjvec = dynamic_cast<DataObjectVec *>(dobj.get())) {
		FinderMessageList *fml = FinderMessageListTypeSupport::create_data();
		
		for (DataObjectVec::Vec::iterator i = dobjvec->vec.begin(); i != dobjvec->vec.end(); ++i) {
			if (FinderResult2D *fres2d = dynamic_cast<FinderResult2D *>(i->get())) {
				int newpos = fml->messages2d.length();
				fml->messages2d.ensure_length(newpos+1, newpos+1);
				
				Finder2DMessage *f2dmsg = Finder2DMessageTypeSupport::create_data();
				f2dmsg->u = fres2d->u;
				f2dmsg->v = fres2d->v;
				f2dmsg->scale = fres2d->scale;
				f2dmsg->angle = fres2d->angle;
				
				fml->messages2d[newpos] = *f2dmsg;
				Finder2DMessageTypeSupport::delete_data(f2dmsg);
			} else if (FinderResult3D *fres3d = dynamic_cast<FinderResult3D *>(i->get())) {
				int newpos = fml->messages3d.length();
				fml->messages3d.ensure_length(newpos+1, newpos+1);
				
				Finder3DMessage *f3dmsg = Finder3DMessageTypeSupport::create_data();
				f3dmsg->x = fres3d->x;
				f3dmsg->y = fres3d->y;
				f3dmsg->z = fres3d->z;
				f3dmsg->ang1 = fres3d->ang1;
				f3dmsg->ang2 = fres3d->ang2;
				f3dmsg->ang3 = fres3d->ang3;
				
				fml->messages3d[newpos] = *f3dmsg;	
				Finder3DMessageTypeSupport::delete_data(f3dmsg);
			}
		}
	
		finderlistsender.Send(*fml);
	}
}
