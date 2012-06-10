import json

from subjugator import topics
from subjugator import sched
from subjugator import nav

import dds

def set_objects(objectnames, cameraid):
    topic = topics.get('VisionSetObjects')
    topic.send({'objectnames': objectnames, 'cameraid': cameraid})

def wait():
    topic = topics.get('VisionResults')
    sched.ddswait(topic)

def get_objects(objectname):
    topic = topics.get('VisionResults') # TODO, multiple camera. Need to change messages and QOS to work correctly here.
    try:
        msg = topic.read()
        return [obj for obj in map(json.loads, msg['messages']) if int(obj['objectName']) == objectid]
    except dds.Error:
        return []

def get_largest_object(objectid, field='scale'):
    objs = vision.get_objects(objectid)
    if len(objs) == 0:
        return None
    return max(objs, lambda obj: float(obj[field]))

class VisualAlgorithm(object):
    def __init__(self, objectid, cameraid):
        self.objectid = objectid
        self.cameraid = cameraid

    def run(self):
        set_ids([objectid], cameraid)
        failctr = 0
        while True:
            vision.wait()
            obj = get_largest_object(self.objectid, self.cameraid)
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
    def __init__(self, objectid, cameraid, fastvel, slowscale, slowvel, maxscale):
        VisualAlgorithm.__init__(self, objectid, cameraid)
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
    def __init__(self, objectid, cameraid, fastvel, slowscale, slowvel, maxscale, ky, kz):
        ForwardVisualAlgorithm.__init__(self, objectid, cameraid, fastvel, slowscale, slowvel, maxscale)
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
    def __init__(self, objectid, cameraid, kY, kz, fastvel, slowscale, slowvel, maxscale):
        ForwardVisualAlgorithm.__init__(self, objectid, cameraid, fastvel, slowscale, slowvel, maxscale)
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
    def __init__(self, objectid, cameraid, kx, ky, kY):
        VisualAlgorithm.__init__(self, objectid, cameraid)
        self.kx = kx
        self.ky = ky
        self.kY = kY

    def update(obj):
        xvel = self.kX*float(obj['u'])
        yvel = self.ky*float(obj['v'])
        Yvel = self.kY*float(obj['angle'])

        if Yvel < 0.005:
            return True

        nav.vel(xvel, yvel, Y=Yvel)
        return False
