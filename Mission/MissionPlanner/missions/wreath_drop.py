from subjugator import sched, nav, vision, sub
from missionplanner import mission

import math
import dds

servo = vision.BottomVisualServo(kx=.2, ky=.2, kz=.004, zmax=.1, desired_scale=230, debug=True)

sel = vision.Selector(vision.DOWN_CAMERA, 'wreath')

def run(drop=True):
    nav.setup()
    nav.depth(.4)

    with sched.Timeout(60) as t:
        while True:
            print 'Looking for wreath'
            nav.vel(.15)
            vision.wait_visible(sel, 5)

            with mission.State('servo'):
                if not servo(sel):
                    print 'Failed to servo on wreath'
                    continue

            break

    if drop:
        nav.down(.6)
        nav.bk(.2)
        sched.sleep(.5)
        sub.Grabber.open()
        sched.sleep(1)
        sub.Grabber.disable()
    return True

mission.missionregistry.register('Wreath-align', lambda: run(False))
mission.missionregistry.register('Wreath-drop', lambda: run(True))
