import math

import dds
from subjugator import nav, sched, vision

BIN_1 = 'shield'
BIN_2 = 'net'

servo = vision.BottomVisualServo(kx=.4, ky=.4, debug=True)
down_servo = vision.BottomVisualServo(kx=.4, ky=.4, kz=.4, zmax=.4, desired_scale=5000, debug=True) # TODO set desired_scale

allbins_sel = vision.Selector(vision.DOWN_CAMERA, 'bins/all')
bin1_sel = vision.Selector(vision.DOWN_CAMERA, 'bins/single', vision.FilterCompare('item', '__eq__', BIN_1))
bin2_sel = vision.Selector(vision.DOWN_CAMERA, 'bins/single', vision.FilterCompare('item', '__eq__', BIN_2))

def run():
    nav.setup()
    print 'Going to .1 depth'
    nav.depth(.1)
    print 'Going forward'
    nav.vel(.2)

    with mission.State('allbins'):
        print 'Waiting for bins to be visible...'
        vision.wait_visible(allbins_sel)

        while not servo(allbins_sel):
            print 'Failed to servo on all'
            nav.vel(.2)
            vision.wait_visible(allbins_sel)
            print 'Servoing on all'

    center_pos = nav.get_trajectory().pos
    print 'Center Pos:', center_pos

    with mission.State('bin1'):
        if not down_servo(bin1_sel):
            print 'Failed to servo on', BIN_1
            return False
        print 'Dropping first marker!'
        sched.sleep(5)
        print 'Done!'

    print 'Returning to center...'
    nav.set_waypoint(nav.Waypoint(center_pos))
    nav.wait()
    print 'Done!'

    with mission.State('bin2'):
        if not down_servo(bin2_sel):
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
