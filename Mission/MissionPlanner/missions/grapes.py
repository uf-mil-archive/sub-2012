from subjugator import sched, nav, vision
from missionplanner import mission

board_servo = vision.StrafeVisualServo(fastvel=.3, slowscale=60000, slowvel=.1, maxscale=85000, ky=.5, kz=.5, yztol=.025, debug=True)
grape_servo = vision.StrafeVisualServo(fastvel=.2, slowscale=40, slowvel=.05, maxscale=80, ky=.1, kz=.1, yztol=.005, debug=True)

board_sel = vision.Selector(vision.FORWARD_CAMERA, 'grapes/board')
horiz_grape_sel = vision.Selector(vision.FORWARD_CAMERA, 'grapes/grape_close', vision.FilterSortKey(lambda obj: float(obj['center'][0]), False))
vert_grape_sel = vision.Selector(vision.FORWARD_CAMERA, 'grapes/grape_close', vision.FilterSortKey(lambda obj: float(obj['center'][0]), True))

grape_count_sel = vision.Selector(vision.FORWARD_CAMERA, 'grapes/grape')

@mission.State("push_horizontal")
def push_horizontal():
    print 'Approaching horizontal grape'
    nav.vel(.2)
    vision.wait_visible(horiz_grape_sel, 2)

    print 'Servoing on horizontal grape'
    if not grape_servo(horiz_grape_sel):
        print 'Failed to servo on horizontal grape'
        return False

    print 'Open loop'
    nav.fd(.6, speed=.05)
    print 'Waiting'
    sched.sleep(5)
    print 'Strafing'
    nav.rstrafe(.2, speed=.1)


@mission.State('push_vertical')
def push_vertical():
    print 'Approaching vertical grape'
    nav.vel(.2)
    vision.wait_visible(vert_grape_sel, 2)

    print 'Servoing on vertical grape'
    if not grape_servo(vert_grape_sel):
        print 'Failed to servo on vertical grape'
        return False

    print 'Open loop'
    nav.down(.1, speed=.1)
    nav.fd(.6, speed=.05)
    print 'Waiting'
    sched.sleep(5)
    print 'Going up'
    nav.up(.2, speed=.1)

def push_retry(push_func, orig_num):
    while True:
        push_func()
        nav.bk(3.5)
        nav.up(.4)

        print 'Re-servoing on board'
        board_servo(board_sel)

        grape_count_sel.setup()
        sched.sleep(1)
        if len(list(grape_count_sel.get_objects())) == orig_num:
            print 'Failed, retrying'
            continue
        break

def run():
    nav.setup()
    nav.depth(1)

    with sched.Timeout(60) as t:
        while True:
            print 'Looking for board'
            nav.vel(.2)
            vision.wait_visible(board_sel, 5)
            sched.sleep(1)

            print 'Servoing to board'
            if not board_servo(board_sel):
                print 'Failed to servo to board'
                continue
            break
    if t.activated:
        print 'Timeout on grape board'
        return False

    print 'Open loop grape approach'
    nav.go(x=1, y=-.1, z=.2, rel=True)

    push_retry(push_horizontal, 2)

    print 'Open loop grape approach'
    nav.go(0, .4, -.3, rel=True)

    push_retry(push_vertical, 1)

    print 'Turning around'
    nav.depth(.5)
    nav.lturn(180)

mission.missionregistry.register('Grapes', run)
