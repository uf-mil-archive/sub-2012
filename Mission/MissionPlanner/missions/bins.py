import math

import dds
from subjugator import nav, sched, vision

servo = vision.BottomVisualServo(kx=.4, ky=.4, debug=True)

BIN_1 = 'shield'
BIN_2 = 'net'

def run():
    nav.setup()
    print 'Going to .1 depth'
    nav.depth(.1, wait=True)
    print 'Going forward'
    nav.vel(.2)
    
    print 'Waiting for bins to be visible...'
    vision.wait_visible('bins/single', vision.DOWN_CAMERA)

    while not servo('bins/all', key_func=lambda obj: 1):
        print 'Failed to servo on all'
        nav.vel(.2)
        vision.wait_visible('bins/all', vision.DOWN_CAMERA)
        print 'Servoing on all'

    center_pos = nav.get_trajectory().pos
    print 'Center Pos:', center_pos
    
    if not servo('bins/single', obj_filter=lambda obj: obj['item'] == BIN_1, key_func=lambda obj: 1):
        print 'Failed to servo on', BIN_1
        return False

    print 'Dropping first marker!'
    nav.down(1)
    sched.sleep(5)
    nav.up(1)
    print 'Done!'

    print 'Returning to center...'
    nav.set_waypoint(nav.Waypoint(center_pos))
    nav.wait()
    print 'Done!'

    if not servo('bins/single', obj_filter=lambda obj: obj['item'] == BIN_2, key_func=lambda obj: 1):
        print 'Failed to servo on', BIN_2
        return False
    print 'Dropping second marker!'
    nav.down(1)
    sched.sleep(5)
    nav.up(1)
    print 'Done!'
    
    return True

if __name__ == '__main__':
    sched.Task('maintask')(run)
    sched.run()
else:
    from missionplanner import mission
    mission.missionregistry.register('Bins', run)
