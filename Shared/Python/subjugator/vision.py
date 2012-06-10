import json

from subjugator import topics
from subjugator import sched
from subjugator import nav

import dds

def set_object_names(objectnames, cameraid):
    topic = topics.get('VisionSetObjects')
    topic.send({'objectnames': objectnames, 'cameraid': cameraid})

def wait():
    topic = topics.get('VisionResults')
    sched.ddswait(topic)

def get_objects(objectnames):
    if isinstance(objectnames, str):
        objectnames = [objectnames]
    topic = topics.get('VisionResults')
    try:
        samples = topic.read_all()
        return [obj for obj in map(json.loads, sample['messages']) for sample in samples if obj['objectName'] in objectnames]
    except dds.Error:
        return []

def get_largest_object(objectid, field='scale'):
    objs = vision.get_objects(objectid)
    if len(objs) == 0:
        return None
    return max(objs, lambda obj: float(obj[field]))

DOWN_CAMERA = 1
FORWARD_CAMERA = 2

class VisualAlgorithm(object):
    def __init__(self, cameraid):
        self.cameraid = cameraid

    def run(self, objectname):
        set_object_names([objectname], cameraid)
        failctr = 0
        while True:
            vision.wait()
            obj = get_largest_object(objectname, self.cameraid)
            if obj is not None:
                if update(obj):
                    break
            else:
                nav.stop()
                failctr += 1
                if failctr > 5:
                    return False
        nav.stop()
    def __call__(self):
        return self.run()

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
    def __init__(self, fastvel, slowscale, slowvel, maxscale, ky, kz):
        ForwardVisualAlgorithm.__init__(self, fastvel, slowscale, slowvel, maxscale)
        self.ky = ky
        self.kz = kz

    def update(self, obj):
        xvel = self._get_vel(obj)
        yvel = self.ky*float(obj['u'])
        zvel = self.kz*float(obj['v'])

        if xvel == 0 and yvel < 0.005 and zvel < 0.005:
            return True

        nav.vel(xvel, yvel, zvel)
        return False

class YawVisualServo(ForwardVisualAlgorithm):
    def __init__(self, kY, kz, fastvel, slowscale, slowvel, maxscale):
        ForwardVisualAlgorithm.__init__(self, fastvel, slowscale, slowvel, maxscale)
        self.kY = kY
        self.kz = kz

    def update(obj):
        xvel = self._get_vel(obj)
        Yvel = self.kY*float(obj['u'])
        zvel = self.kz*float(obj['v'])

        if xvel == 0 and Yvel < 0.005 and zvel < 0.005:
            return True

        nav.vel(xvel, 0, zvel, Y=Yvel)
        return False

class BottomVisualServo(VisualAlgorithm):
    def __init__(self, kx, ky):
        VisualAlgorithm.__init__(self, DOWN_CAMERA)
        self.kx = kx
        self.ky = ky

    def update(obj):
        xvel = self.kX*float(obj['u'])
        yvel = self.ky*float(obj['v'])
        yaw = nav.get_trajectory().Y + float(obj['angle'])

        if Yvel < 0.005:
            return True

        nav.set_waypoint(nav.make_waypoint(Y=yaw, xvel=xvel, yvel=yvel), coordinate=False)
        return False

def wait_visible(objectnames, cameraid, timeout=None):
    if isinstance(objectnames, str):
        objectnames = [objectnames]
    set_object_names(objectnames, cameraid)

    def inner(objectnames=objectnames):
        while True:
            wait()
            objs = get_objects(objectnames)
            if len(objs) > 0:
                return objs

    if timeout is not None: # TODO this is fairly awkward
        with sched.Timeout(timeout):
            return inner()
        return []
    else:
        return inner()
