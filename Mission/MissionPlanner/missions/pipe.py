from subjugator import sched
from subjugator import nav
from subjugator import vision
from missionplanner import mission

import math
import dds

servo = vision.BottomVisualServo(kx=.4, ky=.4, debug=True)

def run():
    nav.setup()
    nav.depth(.5)

    while True:
        print 'Looking for pipe'
        nav.vel(.2)
        vision.wait_visible('pipe', vision.DOWN_CAMERA)
        print 'See pipe!'

        if servo('pipe'):
            break

    print 'Going forward!'
    nav.fd(2)
    return True

mission.missionregistry.register('Pipe', run)
