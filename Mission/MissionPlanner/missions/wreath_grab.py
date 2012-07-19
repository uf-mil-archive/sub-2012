from subjugator import sched, nav, vision, sub
from missionplanner import mission

import math
import dds

servo = vision.BottomVisualServo(kx=.2, ky=.2, kz=.004, zmax=.1, desired_scale=230, debug=True)

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
        nav.bk(.2)
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
        if try_grab(1.5 + .1*i):
            return True
        print "Failed, trying again"
        nav.depth(.2)
    return False

def run():
    nav.setup()
    nav.depth(.4)

    with sched.Timeout(60) as t:
        while True:
            print 'Looking for wreath'
            nav.vel(.2)
            vision.wait_visible(sel, 5)

            print 'Beginning grabs'
            if not repeat_grabs():
                continue
            break
    if t.activated:
        print 'Timeout while finding wreath'
        return False

    return True

mission.missionregistry.register('Wreath-grab', run, 3*60)
