from subjugator import sched, nav, vision
from missionplanner import mission

board_servo = vision.StrafeVisualServo(fastvel=.3, slowscale=10000, slowvel=.05, maxscale=10000, ky=.3, kz=.3, yztol=.05, debug=True)
grape_servo = vision.StrafeVisualServo(fastvel=.3, slowscale=100000, slowvel=.05, maxscale=30000, ky=.3, kz=.3, yztol=.02, debug=True)

board_sel = vision.Selector(vision.FORWARD_CAMERA, 'grapes/board')
horiz_grape_sel = vision.Selector(vision.FORWARD_CAMERA, 'grapes/grape', vision.FilterSortKey(lambda obj: float(obj['center'][0]), False))
vert_grape_sel = vision.Selector(vision.FORWARD_CAMERA, 'grapes/grape', vision.FilterSortKey(lambda obj: float(obj['center'][0]), True))

def run():
    nav.setup()
    nav.depth(1.5)

    print 'Looking for board'
    nav.vel(.2)
    vision.wait_visible(board_sel)
    sched.sleep(1)

    print 'Servoing to board'
    if not board_servo(board_sel):
        print 'Failed to servo to board'
        return False

    print 'Servoing on horizontal grape'
    if not grape_servo(horiz_grape_sel):
        print 'Failed to servo on horizontal grape'
        return False

    print 'Open loop'
    nav.fd(.5, speed=.1)
    nav.lstrafe(.2, speed=.1)
    nav.bk(2)
    nav.rstrafe(.6)

    print 'Servoing on vertical grape'
    if not grape_servo(vertical_grape_sel):
        print 'Failed to servo on vertical grape'
        return False

    print 'Open loop'
    nav.fd(.5, speed=.1)
    nav.up(.5, speed=.1)
    nav.bk(2)

    print 'Turning around'
    nav.depth(.5)
    nav.lturn(180)
