from subjugator import sched, nav, vision, sub
from missionplanner import mission

import math
import dds

servo = vision.BottomVisualServo(kx=.4, ky=.4, kz=.00004, zmax=.2, desired_scale=200, debug=True)

sel = vision.Selector(vision.DOWN_CAMERA, 'wreath')

@mission.State('try_grab {:.2}')
def try_grab(down_dist):
    sub.Grabber.open()

    print 'Servoing'
    with mission.State('servo'):
        if not servo(sel):
            return False

    print 'Open loop grab'
    with mission.State('grab'):
        nav.down(down_dist)
        sched.sleep(.5)
        sub.Grabber.close()
        sched.sleep(1)
        nav.up(down_dist)

    for i in xrange(3): # Ugly, multiple attempts because DDS topics are lazily created
        sched.sleep(.5)
        if sub.Grabber.closed:
            print 'Missed'
            return False

    print 'Success!'
    return True

def repeat_grabs():
    for i in xrange(3):
        if try_grab(.5 + .2*i):
            return True
        nav.up(1)
    return False

def run():
    nav.setup()
    nav.depth(.2)

    print 'Looking for wreath'
    nav.vel(.2)
    vision.wait_visible(sel)

    print 'Beginning grabs'
    if not repeat_grabs():
        return False

    return True

mission.missionregistry.register('Grab Wreath', run)