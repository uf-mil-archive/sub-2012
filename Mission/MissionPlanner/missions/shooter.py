from subjugator import sched, nav, sub, vision
from missionplanner import mission
import math
import dds

servo = vision.StrafeVisualServo(fastvel=.3,
                                 slowscale=10000,
                                 slowvel=.05,
                                 maxscale=55000,
                                 ky=.15,
                                 kz=.15,
                                 debug=True)

small_sels = dict(red=vision.Selector(vision.FORWARD_CAMERA, 'shooter/red/small'),
                  blue=vision.Selector(vision.FORWARD_CAMERA, 'shooter/blue/small'))
box_sels = dict(red=vision.Selector(vision.FORWARD_CAMERA, 'shooter/red/small'),
                blue=vision.Selector(vision.FORWARD_CAMERA, 'shooter/blue/small'))

shooters = dict(red=sub.RightShooter,
                blue=sub.LeftShooter)

any_box_sel = vision.combine_selectors(list(box_sels.itervalues()))

@mission.State('approach_shoot {}')
def approach_shoot(color):
    sel = small_sels[color]

    with sched.Timeout(60) as t:
        while True:
            print 'Forward until small ' + color + ' seen'
            nav.vel(.2)
            vision.wait_visible(sel)

            print 'Servoing for ' + color
            if not servo(sel):
                continue
            break
    if t.activated:
        print 'Timeout while looking for shooter'
        return False


    print 'Getting close'
    nav.go(.5, .1 if color == 'blue' else -.1, -.15, rel=True, speed=.1)
    print 'Shooting'
    sched.sleep(2)
    shooters[color].shoot()
    sched.sleep(1)
    print 'Backing up'
    nav.bk(.5)
    return True

def run():
    nav.setup()
    sched.sleep(.5)
    nav.depth(3)

    print 'Looking for box'
    with mission.State('forward'):
        nav.vel(.2)
        vision.wait_visible(any_box_sel, 5)
    print 'Getting closer'
    sched.sleep(1)

    if box_sels['red'].is_visible():
        firstcolor = 'red'
        secondcolor = 'blue'
    else:
        firstcolor = 'blue'
        secondcolor = 'red'

    if not approach_shoot(firstcolor):
        return False

    with mission.State('over'):
        print 'Going over window'
        origdepth = nav.get_waypoint().pos.z
        nav.depth(1)
        nav.fd(4)
        nav.lturn(180)
        nav.depth(origdepth)

    if not approach_shoot(secondcolor):
        return False

    return True

mission.missionregistry.register('Shooter', run, 3*60)
