from . import topics
from . import mathutils
from . import sched
import dds

import numpy
import functools
import math

def setup():
    settopic = topics.get('SetWaypoint')
    trajtopic = topics.get('Trajectory')

    while True:
        try:
            settopic.read()
            trajtopic.read()
            return
        except dds.Error:
            pass
        sched.sleep(.1) # todo multi-topic ddswait

def make_waypoint(x=0, y=0, z=0, R=0, P=0, Y=0):
    return Waypoint([x, y, z, R, P, Y])

class Waypoint(object):
    def __init__(self, array):
        self.array = numpy.array(array, dtype=float)
        self._transform = None
        self._inverse_transform = None

    @property
    def transform(self):
        if self._transform is None:
            self._transform = mathutils.rpy_to_mat4(self.RPY).transpose().dot(mathutils.pos_to_mat4(-self.xyz))
        return self._transform

    @property
    def inverse_transform(self):
        if self.inv_T is None:
            self._inverse_transform = mathutils.pos_to_mat4(self.xyz).dot(mathutils.rpy_to_mat4(self.RPY))
        return self._inverse_transform

    _char_positions = dict(zip("xyzRPY", xrange(6)))

    # Implements swizzling attributes, all permutations of xyzRPY like .xy, .RPY, .z, etc.
    def __getattr__(self, name):
        if not all(ch in Waypoint._char_positions for ch in name):
            return
        vals = []
        for ch in name:
            pos = Waypoint._char_positions.get(ch)
            vals.append(self.array[pos])
        if len(vals) == 1:
            return vals[0]
        else:
            return numpy.array(vals)

    def __setattr__(self, name, value):
        if not all(ch in Waypoint._char_positions for ch in name):
            return object.__setattr__(self, name, value)
        if isinstance(value, float) or isinstance(value, int):
            value = [value]
        assert len(name) == len(value)
        for valpos in xrange(len(value)):
            pos = Waypoint._char_positions.get(name[valpos], -1)
            if pos == -1:
                raise AttributeError()
            self.array[pos] = value[valpos]

    def resolve_relative(self, basepoint):
        waypoint = numpy.empty(6)
        waypoint[0:3] = mathutils.from_homog(basepoint.inverse_transform.dot(mathutils.to_homog(self.xyz)))
        waypoint[3:6] = mathutils.angle_wrap_vec(basepoint.RPY + self.RPY)
        return Waypoint(waypoint)

    def relative_from(self, basepoint):
        relpoint = numpy.empty(6)
        relpoint[0:3] = mathutils.from_homog(basepoint.transform.dot(mathutils.to_homog(self.xyz)))
        relpoint[3:6] = mathutils.angle_wrap_vec(-basepoint.RPY + self.RPY)
        return Waypoint(relpoint)

    def approx_equal(self, otherpoint, pos_tol=.01, rad_tol=.01):
        for i in xrange(0, 3):
            if math.fabs(otherpoint.array[i] - self.array[i]) > pos_tol:
                return False
            if mathutils.angle_wrap(math.radians(math.fabs(otherpoint.array[i+3] - self.array[i+3]))) > rad_tol:
                return False
        return True

    def __str__(self):
        return "[%f,%f,%f,%f,%f,%f]" % tuple(self.array)

    def __repr__(self):
        return "Waypoint(" + str(self) + ")"

def set_waypoint(waypoint):
    topic = topics.get('SetWaypoint')
    topic.send({'timestamp': 0, 'position_ned': list(waypoint.xyz), 'rpy': list(waypoint.RPY), 'isRelative': False})

def set_waypoint_rel(relpoint):
    set_waypoint(relpoint.resolve_relative(get_waypoint()))

def get_waypoint():
    topic = topics.get('SetWaypoint')
    try:
        msg = topic.read()
        waypoint = make_waypoint()
        waypoint.xyz = msg['position_ned']
        waypoint.RPY = msg['rpy']
        return waypoint
    except dds.Error:
        raise RuntimeError('No waypoint set')

def get_trajectory():
    topic = topics.get('Trajectory')
    try:
        return Waypoint(topic.read()['xd'])
    except dds.Error:
        raise RuntimeError('No current trajectory')

def wait():
    while True:
        waypoint = get_waypoint()
        trajectory = get_trajectory()
        if trajectory.approx_equal(waypoint):
            return
        sched.ddswait(topics.get('Trajectory'))

def waitopts(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        waitflag = kwargs.get('wait', True)
        if 'wait' in kwargs:
            del kwargs['wait']
        val = func(*args, **kwargs)
        if waitflag:
            wait()
        return val
    return wrapper

@waitopts
def fd(dist):
    set_waypoint_rel(make_waypoint(x=dist))

@waitopts
def bk(dist):
    fd(-dist)

@waitopts
def rstrafe(dist):
    set_waypoint_rel(make_waypoint(y=dist))

@waitopts
def lstrafe(dist):
    rstrafe(-dist)

@waitopts
def down(dist):
    set_waypoint_rel(make_waypoint(z=dist))

@waitopts
def up(dist):
    down(-dist)

@waitopts
def rturn(deg):
    set_waypoint_rel(make_waypoint(Y=math.radians(deg)))

@waitopts
def lturn(deg):
    rturn(-deg)

@waitopts
def depth(depth):
    waypoint = get_waypoint()
    waypoint.z = depth
    set_waypoint(waypoint)

@waitopts
def heading(deg=None, rad=None):
    assert(deg is not None or rad is not None)
    waypoint = get_waypoint()
    if deg is not None:
        waypoint.Y = math.radians(deg)
    else:
        waypoint.Y = rad
    set_waypoint(waypoint)

def go(x, y, z=None, rel=False, base=None):
    curpoint = get_waypoint()
    waypoint = make_waypoint(x, y)
    if rel:
        base = curpoint
    if base is not None:
        waypoint = waypoint.resolve_relative(base)
    else:
        waypoint.z = curpoint.z # z is always relative
    if z is not None:
        waypoint.z = z
    waypoint.RP = curpoint.RP
    waypoint.Y = math.atan2(waypoint.y - curpoint.y, waypoint.x - curpoint.x)
    heading(rad=waypoint.Y)
    set_waypoint(waypoint)
    wait()

def go_seq(points, rel=False, autoyaw=True, base=None):
    if rel:
        base = get_waypoint()
    for point in points:
        go(*point, base=base, autoyaw=autoyaw)
