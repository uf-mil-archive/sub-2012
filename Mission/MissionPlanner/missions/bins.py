import math

import dds
from subjugator import nav, sched, vision, sub
from missionplanner import mission

BIN_1 = 'shield'
BIN_2 = 'sword'

servo = vision.BottomVisualServo(kx=.4, ky=.4, debug=True)
down_servo = vision.BottomVisualServo(kx=.4, ky=.4, kz=.00004, zmax=.2, desired_scale=16000, debug=True)

allbins_sel = vision.Selector(vision.DOWN_CAMERA, 'bins/all')
bin1_sel = vision.Selector(vision.DOWN_CAMERA, 'bins/single', vision.FilterCompare('item', '__eq__', BIN_1))
bin2_sel = vision.Selector(vision.DOWN_CAMERA, 'bins/single', vision.FilterCompare('item', '__eq__', BIN_2))

@mission.State('dropball')
def dropball(sel):
    if not down_servo(sel):
        print 'Failed to servo'
        return False
    print 'Dropping marker!'
    nav.depth(2.5)
    nav.lstrafe(.05)
    sched.sleep(1)
    sub.BallDropper.drop()
    sched.sleep(.5)
    print 'Done!'

@mission.State('allbins')
def allbins():
    print 'Waiting for bins to be visible...'
    nav.vel(.2)
    vision.wait_visible(allbins_sel, 5)

    if not servo(allbins_sel):
        print 'Failed to servo on all'
        return (False, 'servo')

    obj = allbins_sel.get_object()
    print 'Number of boxes', obj['number_of_boxes']
    if obj is not None and int(obj['number_of_boxes']) == 4:
        return (True, None)
    else:
        print 'Not enough boxes, strafing'
        return (False, 'strafe')

def run():
    nav.setup()
    print 'Going to depth'
    nav.depth(.2)

    with sched.Timeout(90) as t:
        left = True
        while True:
            (ok, failtype) = allbins()
            if ok:
                break
            if failtype == 'strafe':
#                t.cancel()
                if left:
                    nav.lstrafe(.25)
                else:
                    nav.rstrafe(.25)
                left = not left

    if t.activated:
        print 'Timed out looking for all bins'
        return False

    nav.down(.2)
    center_pos = nav.get_trajectory().pos
    print 'Center Pos:', center_pos

    dropball(bin1_sel)

    print 'Returning to center...'
    nav.set_waypoint(nav.Waypoint(center_pos))
    nav.wait()
    sched.sleep(1.5)
    print 'Done!'

    dropball(bin2_sel)

    print 'Returning to center...'
    waypoint = nav.Waypoint(center_pos)
    waypoint.pos.z = 2.5
    nav.set_waypoint(waypoint)
    nav.wait()
    return True

if __name__ == '__main__':
    sched.Task('maintask')(run)
    sched.run()
else:
    from missionplanner import mission
    mission.missionregistry.register('Bins', run, 2*60)
