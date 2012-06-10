from subjugator import topics
from subjugator import sched
from subjugator import nav

import json

# IMO we should change from IDs to strings
OBJECTID_BUOY_YELLOW = 5
OBJECTID_BUOY_RED = 6
OBJECTID_BUOY_GREEN = 7
OBJECTID_PIPE = 8

def set_ids(objectids, cameraid):
    topic = topics.get('VisionSetIDs')
    topic.send({'visionids': objectids, 'cameraid': cameraid})

def wait():
    topic = topics.get('Vision')
    sched.ddswait(topic)

def get_objects(objectid):
    topic = topics.get('Vision') # TODO, multiple camera. Need to change messages and QOS to work correctly here.
    try:
        msg = topic.read()
        return [obj for obj in map(json.loads, msg['messages']) if int(obj['objectID']) == objectid]
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
        yvel = self.ky*(int(obj['u'])-320)
        zvel = self.kz*(int(obj['v'])-240)

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
        Yvel = self.kY*(int(obj['u'])-320)
        zvel = self.kz*(int(obj['v'])-240)

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
        xvel = self.kX*(int(obj['u'])-320)
        yvel = self.ky*(int(obj['v'])-240)
        Yvel = self.kY*float(obj['angle'])

        if Yvel < 0.005:
            return True

        nav.vel(xvel, yvel, Y=Yvel)
        return False
