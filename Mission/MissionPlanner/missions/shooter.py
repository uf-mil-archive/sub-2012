from subjugator import sched, nav, sub, vision
from missionplanner import mission
import math
import dds

servo = vision.StrafeVisualServo(fastvel=.3,
                                 slowscale=10000,
                                 slowvel=.05,
                                 maxscale=60000,
                                 ky=.15,
                                 kz=.15,
                                 debug=True)

small_sels = dict(red=vision.Selector(vision.FORWARD_CAMERA, 'shooter/red/small'),
                  blue=vision.Selector(vision.FORWARD_CAMERA, 'shooter/blue/small'))
box_sels = dict(red=vision.Selector(vision.FORWARD_CAMERA, 'shooter/red/box'),
                blue=vision.Selector(vision.FORWARD_CAMERA, 'shooter/blue/box'))

shooters = dict(red=sub.RightShooter,
                blue=sub.LeftShooter)

any_box_sel = vision.combine_selectors(list(box_sels.itervalues()))

@mission.State('approach_shoot')
def approach_shoot(color):
    sel = small_sels[color]

    print 'Forward until small ' + color + ' seen'
    nav.vel(.2)
    vision.wait_visible(sel)

    print 'Servoing for ' + color
    if not servo(sel):
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
    nav.depth(1)

    with mission.State('forward'):
        print 'Forward until shooter seen'
        nav.vel(.2)
        vision.wait_visible(any_box_sel)
        print 'Getting closer'
        sched.sleep(1)
        vision.wait_visible(any_box_sel)

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
        nav.depth(.5)
        nav.fd(4)
        nav.lturn(180)
        nav.depth(origdepth)

    if not approach_shoot(secondcolor):
        return False

    nav.depth(.5)
    nav.fd(1.5)
    nav.rturn(80)
    nav.fd(1)
    return True

mission.missionregistry.register('Shooter', run)
