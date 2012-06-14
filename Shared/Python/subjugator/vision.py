import json

from subjugator import topics
from subjugator import sched
from subjugator import nav
from subjugator import mathutils

import dds

def set_object_names(objectnames, cameraid):
    topic = topics.get('VisionSetObjects')
    topic.send({'objectnames': objectnames, 'cameraid': cameraid})

def wait():
    topic = topics.get('VisionResults')
    sched.ddswait(topic)

def get_objects(objectnames, cameraid):
    if isinstance(objectnames, str):
        objectnames = [objectnames]
    topic = topics.get('VisionResults')
    try:
        msgs = topic.read_all()
        return [obj for msg in msgs for obj in map(json.loads, msg['messages']) if obj['objectName'] in objectnames if msg['cameraid'] == cameraid]
    except dds.Error:
        return []

def get_largest_object(objectid, cameraid, field='scale'):
    objs = get_objects(objectid, cameraid)
    if len(objs) == 0:
        return None
    return max(objs, key=lambda obj: float(obj[field]))

DOWN_CAMERA = 0
FORWARD_CAMERA = 1

class VisualAlgorithm(object):
    def __init__(self, cameraid):
        self.cameraid = cameraid

    def run(self, objectname):
        set_object_names([objectname], self.cameraid)
        failctr = 0
        while True:
            wait()
            obj = get_largest_object(objectname, self.cameraid)
            if obj is not None:
                if self.update(obj):
                    break
            else:
                nav.stop()
                failctr += 1
                if failctr > 5:
                    return False
        nav.stop()
        return True

    def __call__(self, objectname):
        return self.run(objectname)

    def update(self):
        raise NotImplementedError()

class ForwardVisualAlgorithm(VisualAlgorithm):
    def __init__(self, fastvel, slowscale, slowvel, maxscale):
        VisualAlgorithm.__init__(self, FORWARD_CAMERA)
        self.fastvel = fastvel
        self.slowscale = slowscale
        self.slowvel = slowvel
        self.maxscale = maxscale

    def _get_vel(self, obj):
        scale = float(obj['scale'])
        if scale < self.slowscale:
            return self.fastvel
        elif scale < self.maxscale:
            return self.slowvel
        else:
            return 0

class StrafeVisualServo(ForwardVisualAlgorithm):
    def __init__(self, fastvel, slowscale, slowvel, maxscale, ky, kz, debug=False):
        ForwardVisualAlgorithm.__init__(self, fastvel, slowscale, slowvel, maxscale)
        self.ky = ky
        self.kz = kz
        self.debug = debug

    def update(self, obj):
        xvel = self._get_vel(obj)
        yvel = self.ky*float(obj['center'][0])
        zvel = -self.kz*float(obj['center'][1])

        if xvel == 0 and yvel < 0.005 and zvel < 0.005:
            return True

        if self.debug:
            print 'xvel', xvel, 'yvel', yvel, 'zvel', zvel
        nav.vel(xvel, yvel, zvel)
        return False

class YawVisualServo(ForwardVisualAlgorithm):
    def __init__(self, kY, kz, fastvel, slowscale, slowvel, maxscale, debug=False):
        ForwardVisualAlgorithm.__init__(self, fastvel, slowscale, slowvel, maxscale)
        self.kY = kY
        self.kz = kz
        self.debug = debug

    def update(self, obj):
        xvel = self._get_vel(obj)
        Yvel = self.kY*float(obj['center'][0])
        zvel = self.kz*float(obj['center'][1])

        if xvel == 0 and Yvel < 0.005 and zvel < 0.005:
            return True

        if self.debug:
            print 'xvel', xvel, 'Yvel', Yvel
        nav.vel(xvel, 0, zvel, Y=Yvel)
        return False

class BottomVisualServo(VisualAlgorithm):
    def __init__(self, kx, ky, debug):
        VisualAlgorithm.__init__(self, DOWN_CAMERA)
        self.kx = kx
        self.ky = ky
        self.debug = debug

    def update(self, obj):
        xvel = self.kx*float(obj['center'][1])
        yvel = self.ky*float(obj['center'][0])
        yaw = mathutils.angle_wrap(nav.get_trajectory().Y + float(obj['angle']))

        if abs(float(obj['angle'])) < math.degrees(2) and xvel < 0.005 and yvel < 0.005:
            return True

        if self.debug:
            print 'xvel', xvel, 'yvel', yvel, 'Y', yaw
        nav.set_waypoint(nav.make_waypoint(Y=yaw, xvel=xvel, yvel=yvel), coordinate=False)
        return False

def wait_visible(objectnames, cameraid, timeout=None):
    if isinstance(objectnames, str):
        objectnames = [objectnames]
    set_object_names(objectnames, cameraid)

    def inner(objectnames=objectnames):
        while True:
            wait()
            objs = get_objects(objectnames, cameraid)
            if len(objs) > 0:
                return objs

    if timeout is not None: # TODO this is fairly awkward
        with sched.Timeout(timeout):
            return inner()
        return []
    else:
        return inner()
