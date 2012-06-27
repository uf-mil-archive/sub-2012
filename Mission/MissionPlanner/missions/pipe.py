from subjugator import sched
from subjugator import nav
from subjugator import vision
from missionplanner import mission

import math
import dds

servo = vision.BottomVisualServo(kx=.4, ky=.4, debug=True)

pipe_sels = dict(any=vision.Selector(vision.DOWN_CAMERA, 'pipe'),
                 left=vision.Selector(vision.DOWN_CAMERA, 'pipe', vision.FilterSort('angle', descending=False)),
                 right=vision.Selector(vision.DOWN_CAMERA, 'pipe', vision.FilterSort('angle', descending=True)))

def run(name):
    nav.setup()
    nav.depth(.2)

    while True:
        print 'Looking for ' + name + ' pipe'
        nav.vel(.2)
        vision.wait_visible(pipe_sels['name'])
        print 'See pipe!'

        with mission.State('servo'):
            if servo(pipe_sel):
                break
    return True

mission.missionregistry.register('Pipe', lambda: run('any'))
mission.missionregistry.register('Pipe-left', lambda: run('left'))
mission.missionregistry.register('Pipe-right', lambda: run('right'))
