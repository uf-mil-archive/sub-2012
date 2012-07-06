from subjugator import sched, nav, vision, sub
from missionplanner import mission

import math
import dds

servo = vision.BottomVisualServo(kx=.2, ky=.2, kz=.004, zmax=.1, desired_scale=230, debug=True)

sel = vision.Selector(vision.DOWN_CAMERA, 'wreath')

def try_grab():
    print 'Open loop grab'
    sub.Grabber.open()
    sched.sleep(.5)
    nav.down(.5)
    sched.sleep(1)
    sub.Grabber.close()
    sched.sleep(2)
    nav.up(.3)
    sched.sleep(1)

    if sub.Grabber.closed:
        print 'Missed'
        return False

    print 'Success!'
    return True

def repeat_grabs():
    print 'Servoing'
    with mission.State('servo'):
        if not servo(sel):
            return False

    nav.down(.5)

    for i in xrange(3):
        with mission.State('try_grab ' + str(i)):
            if try_grab():
                return True

    return False

def run():
    nav.setup()
    nav.depth(.2)

    sub.Grabber.closed # Dummy read to get DDS in shape

    print 'Looking for wreath'
    nav.vel(.2)
    vision.wait_visible(sel)

    print 'Beginning grabs'
    if not repeat_grabs():
        return False

    print 'Coming up'
    nav.depth(.2)
    return True

mission.missionregistry.register('Grab Wreath', run)
