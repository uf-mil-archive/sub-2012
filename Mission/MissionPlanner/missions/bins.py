import math

import dds
from subjugator import nav, sched, vision

servo = vision.BottomVisualServo(kx=.4, ky=.4, debug=True)

BIN_1 = 'sword'
BIN_2 = 'shield'

def run():
    nav.setup()
    nav.vel(.2)
    
    print 'Waiting for bins to be visible...'
    vision.wait_visible('bins/all', vision.DOWN_CAMERA)
    print 'See all!'
    if not servo('bins/all'):
        print 'Failed to servo on all'
        return False
    center_pos = nav.get_trajectory().pos
    print center_pos
    
    if not servo('bins/single', lambda obj: obj['item'] == BIN_1):
        print 'Failed to servo on', BIN_1
        return False
    print 'Dropping first marker!'
    sched.sleep(5)
    print 'Done!'
    
    print 'Returning to center...'
    nav.set_waypoint(nav.Waypoint(center_pos))
    nav.wait()
    print 'Done!'

    if not servo('bins/single', lambda obj: obj['item'] == BIN_2):
        print 'Failed to servo on', BIN_2
        return False
    print 'Dropping second marker!'
    sched.sleep(5)
    print 'Done!'
    
    return True

if __name__ == '__main__':
    sched.Task('maintask')(run)
    sched.run()
else:
    from missionplanner import mission
    mission.missionregistry.register('Bins', run)
