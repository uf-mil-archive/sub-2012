import json

from subjugator import topics
from subjugator import sched
from subjugator import nav
from subjugator import mathutils

import dds
import math

def set_object_names(object_names, camera_id):
    if not isinstance(object_names, list):
        object_names = [object_names]
    topic = topics.get('VisionSetObjects')
    topic.send({'objectnames': object_names, 'cameraid': camera_id})

def wait():
    topic = topics.get('VisionResults')
    sched.ddswait(topic)

def get_objects(object_names, camera_id):
    if not isinstance(object_names, list):
        object_names = [object_names]
    topic = topics.get('VisionResults')
    try:
        msgs = topic.read_all()
        for msg in msgs:
            if msg['cameraid'] == camera_id:
                objs = map(json.loads, msg['messages'])
                return [obj for obj in objs if obj['objectName'] in object_names]
        return []
    except dds.Error:
        return []

class Filter(object):
    def __call__(self, objs):
        return objs

class FilterChain(Filter):
    def __init__(self, *args):
        self.filters = args

    def __call__(self, objs):
        for filt in self.filters:
            objs = filt(objs)
        return objs

class FilterSum(Filter):
    def __init__(self, *args):
        self.filters = args

    def __call__(self, objs):
        sum_objs = []
        for filt in self.filters:
            for obj in filt(objs):
                if obj not in sum_objs:
                    sum_objs.append(obj)
        return iter(sum_objs)

class FilterCompare(Filter):
    def __init__(self, field_name, op_name, val):
        self.field_name = field_name
        self.op_name = op_name
        self.val = val

    def __call__(self, objs):
        val_type = type(self.val)
        op = getattr(val_type, self.op_name)
        return (obj for obj in objs if op(val_type(obj[self.field_name]), self.val))

class FilterSort(Filter):
    def __init__(self, field_name, descending=True):
        self.field_name = field_name
        self.descending = descending

    def __call__(self, objs):
        return sorted(objs, key=key, reverse=self.descending)

class FilterScore(Filter):
    def __init__(self, score_func, min_score=0):
        self.score_func = score_func
        self.min_score = min_score

    def __call__(self, objs):
        for obj in sorted(objs, key=self.score_func, reverse=True):
            if self.score_func(obj) > self.min_score:
                yield obj
            else:
                break

class FilterLimit(Filter):
    def __init__(self, limit):
        self.limit = limit

    def __call__(self, objs):
        for num, obj in enumerate(objs):
            if num < self.limit:
                yield obj
            else:
                break

class Selector(object):
    def __init__(self, camera_id, object_names, object_filter=Filter()):
        self.camera_id = camera_id
        if not isinstance(object_names, list):
            object_names = [object_names]
        self.object_names = object_names
        self.object_filter = object_filter

    def setup(self):
        set_object_names(self.object_names, self.camera_id)

    def get_objects(self):
        objs = get_objects(self.object_names, self.camera_id)
        print objs
        return self.object_filter(iter(objs))

    def get_object(self):
        return next(self.get_objects(), None)

def combine_selectors(sels, filt=Filter()):
    camera_id = sels[0].camera_id
    object_names = set()
    object_filters = []
    for sel in sels:
        assert camera_id == sel.camera_id
        object_names.update(sel.object_names)
        object_filters.append(sel.object_filter)
    return Selector(camera_id, list(object_names), FilterChain(FilterSum(*object_filters), filt))

DOWN_CAMERA = 0
FORWARD_CAMERA = 1

class VisualAlgorithm(object):
    def run(self, selector):
        self.setup()
        selector.setup()

        failctr = 0
        while True:
            wait()
            obj = selector.get_object()
            if obj is None:
                failctr += 1
                if failctr > 10:
                    return False
                else:
                    continue
            failctr = 0
            if self.update(obj):
                break
        nav.stop()
        return True

    def __call__(self, *args, **kwargs):
        return self.run(*args, **kwargs)

    def setup(self):
        pass

    def update(self):
        raise NotImplementedError()

class ForwardVisualAlgorithm(VisualAlgorithm):
    def __init__(self, fastvel, slowscale, slowvel, maxscale):
        VisualAlgorithm.__init__(self)
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
    def __init__(self, ky, kz, debug=False, *args, **kwargs):
        ForwardVisualAlgorithm.__init__(self, *args, **kwargs)
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
    def __init__(self, kY, kz, debug=False, *args, **kwargs):
        ForwardVisualAlgorithm.__init__(self, *args, **kwargs)
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
    def __init__(self, kx, ky, kz=0, zmax=0, desired_scale=0, debug=False):
        VisualAlgorithm.__init__(self)
        self.kx = kx
        self.ky = ky
        self.kz = kz
        self.zmax = zmax
        self.desired_scale = desired_scale
        self.debug = debug

    def update(self, obj):
        xvel = self.kx*float(obj['center'][1])
        yvel = self.ky*float(obj['center'][0])
        mostly_centered = abs(xvel) < .15 and abs(yvel) < .15

	if mostly_centered:
            yaw = float(obj['angle'])
        else:
            yaw = 0

        if mostly_centered and self.zmax > 0:
            zvel = self.kz*(self.desired_scale - float(obj['scale']))
            zvel = max(min(zvel, self.zmax), -self.zmax)
        else:
            zvel = 0

        if abs(float(obj['angle'])) < .08 and abs(xvel) < 0.03 and abs(yvel) < 0.03 and abs(zvel) < 0.03:
            return True

        if self.debug:
            print 'xvel', xvel, 'yvel', yvel, 'zvel', zvel, 'Y', yaw
        nav.set_waypoint_rel(nav.make_waypoint(Y=yaw, velx=xvel, vely=yvel, velz=zvel), coordinate=False)
        return False

def wait_visible(selector):
    selector.setup()

    while selector.get_object() == None:
        wait()
