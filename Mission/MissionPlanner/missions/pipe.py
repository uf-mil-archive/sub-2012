from subjugator import sched
from subjugator import nav
from subjugator import vision
from missionplanner import mission

import math
import dds

servo = vision.BottomVisualServo(kx=.4, ky=.4, debug=True)

pipe_sel = vision.Selector(vision.FORWARD_CAMERA, 'pipe')

def run():
    nav.setup()
    nav.depth(.5)

    while True:
        print 'Looking for pipe'
        nav.vel(.2)
        vision.wait_visible(pipe_sel)
        print 'See pipe!'

        with mission.State('servo'):
            if servo(pipe_sel):
                break

    print 'Going forward!'
    nav.fd(2)
    return True

mission.missionregistry.register('Pipe', run)
