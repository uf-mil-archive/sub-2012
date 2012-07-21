from subjugator import sched, nav, sub, vision
from missionplanner import mission
import math
import dds

from . import hydrophone
from . import wreath_grab
from . import wreath_drop
from . import surface

occluded_sel = vision.Selector(vision.DOWN_CAMERA, 'wreath')

def run(practice):
    nav.setup()
    with mission.State('hydro1'):
        if not hydrophone.run(practice):
            return False

    with mission.State('wreath_grab'):
        if not wreath_grab.run():
            print 'Failed to grab'

    occluded_sel.setup()
    sched.sleep(.5)
    if occluded_sel.get_object() != None:
        print 'Vision occluded'
        occluded = True
    else:
        occluded = False

    if not practice:
        print 'Practice mission, dropping'
        sub.Grabber.open()
        return True

    with mission.State('hydro2'):
        if not hydrophone.run(practice):
            return False

    if not occluded:
        with mission.State('drop'):
            wreath_drop.run()
    else:
        print 'Occluded open loop drop'
        nav.fd(1)
        nav.depth(2)
        sub.Grabber.open()

    return True

mission.missionregistry.register('HydroGrabDrop-competition', lambda: run(False), 12*60)
mission.missionregistry.register('HydroGrabDrop-practice', lambda: run(True), 12*60)
