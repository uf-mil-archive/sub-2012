from subjugator import sched, nav, vision, sub
from missionplanner import mission

import math
import dds

servo = vision.BottomVisualServo(kx=.2, ky=.2, kz=.004, zmax=.1, desired_scale=230, debug=True)

sel = vision.Selector(vision.DOWN_CAMERA, 'wreath')

def run():
    nav.setup()
    nav.depth(.4)

    with sched.Timeout(60) as t:
        while True:
            print 'Looking for wreath'
            nav.vel(.2)
            vision.wait_visible(sel, 5)

            with misison.State('servo'):
                if not servo(sel):
                    print 'Failed to servo on wreath'
                    continue

            break

    nav.down(.6)
    sched.sleep(1)
    sub.Grabber.open()
    sched.sleep(1)
    sub.Grabber.disable()

mission.missionregistry.register('Wreath-drop', run)
