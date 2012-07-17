import math

import dds
from subjugator import nav, sched, vision, sub

BIN_1 = 'trident'
BIN_2 = 'shield'

servo = vision.BottomVisualServo(kx=.4, ky=.4, debug=True)
down_servo = vision.BottomVisualServo(kx=.4, ky=.4, kz=.00004, zmax=.2, desired_scale=16000, debug=True)

allbins_sel = vision.Selector(vision.DOWN_CAMERA, 'bins/all')
bin1_sel = vision.Selector(vision.DOWN_CAMERA, 'bins/single', vision.FilterCompare('item', '__eq__', BIN_1))
bin2_sel = vision.Selector(vision.DOWN_CAMERA, 'bins/single', vision.FilterCompare('item', '__eq__', BIN_2))

def run():
    nav.setup()
    print 'Going to .4 depth'
    nav.depth(.4)
    print 'Going forward'
    nav.vel(.2)

    with sched.Timeout(60) as t:
        with mission.State('allbins'):
            print 'Waiting for bins to be visible...'
            vision.wait_visible(allbins_sel)

            while not servo(allbins_sel):
                print 'Failed to servo on all'
                nav.vel(.2)
                vision.wait_visible(allbins_sel)
                print 'Servoing on all'
    if t.activated:
        print 'Timed out looking for all bins'
        return False

    nav.down(.2)
    center_pos = nav.get_trajectory().pos
    print 'Center Pos:', center_pos

    with mission.State('bin1'):
        if not down_servo(bin1_sel):
            print 'Failed to servo on', BIN_1
            return False
        print 'Dropping first marker!'
        nav.down(1)
        nav.lstrafe(.1)
        sched.sleep(1)
        sub.BallDropper.drop()
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
        nav.down(1)
        nav.lstrafe(.1)
        sched.sleep(1)
        sub.BallDropper.drop()
        print 'Done!'

    print 'Returning to center...'
    nav.set_waypoint(nav.Waypoint(center_pos))
    nav.wait()
    return True

if __name__ == '__main__':
    sched.Task('maintask')(run)
    sched.run()
else:
    from missionplanner import mission
    mission.missionregistry.register('Bins', run, 2*60)
