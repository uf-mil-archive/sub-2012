from subjugator import sched, nav, vision, sub
from missionplanner import mission

import math
import dds

servo = vision.BottomVisualServo(kx=.4, ky=.4, zmax=.1, desired_scale=200, debug=True)

sel = vision.Selector(vision.DOWN_CAMERA, 'wreath')

def run():
    nav.setup()
    nav.depth(.2)

    while True:
        print 'Looking for wreath'
        nav.vel(.2)
        vision.wait_visible(sel)
        print 'See wreath!'

        with mission.State('servo'):
            if servo(sel):
                break
    
    sub.Grabber.open()
    nav.down(0.5) # ??? do we have a switch that can detect contact?
    sub.Grabber.close()
    nav.depth(.2)
    nav.vel(.2)
    
    return True

mission.missionregistry.register('Wreath', run)
