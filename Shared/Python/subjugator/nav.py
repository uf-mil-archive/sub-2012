from . import topics
from . import mathutils
from . import sched
import dds

import numpy
from scipy import linalg
import functools
import math

def setup():
    wpttopic = topics.get('Waypoint')
    trajtopic = topics.get('Trajectory')

    while True:
        try:
            trajtopic.read()
            break
        except dds.Error:
            sched.ddswait(trajtopic)

    sched.sleep(1) # TODO: Without this we seem to end up with old trajectory messages? DDS, how does it work?
    set_waypoint(Waypoint(get_trajectory().pos))

    while True:
        try:
            wpttopic.read()
            break
        except dds.Error:
            sched.ddswait(wpttopic)

def make_point(x=0, y=0, z=0, R=0, P=0, Y=0):
    return Point([x, y, z, R, P, Y])

class Point(object):
    def __init__(self, array):
        self.array = numpy.array(array, dtype=float)
        self._transform = None
        self._inverse_transform = None

    _char_positions = dict(zip("xyzRPY", xrange(6)))

    # Implements swizzling attributes, all permutations of xyzRPY like .xy, .RPY, .z, etc.
    def __getattr__(self, name):
        if not all(ch in Point._char_positions for ch in name):
            return
        vals = []
        for ch in name:
            pos = Point._char_positions.get(ch)
            vals.append(self.array[pos])
        if len(vals) == 1:
            return vals[0]
        else:
            return numpy.array(vals)

    def __setattr__(self, name, value):
        if not all(ch in Point._char_positions for ch in name):
            return object.__setattr__(self, name, value)
        if isinstance(value, float) or isinstance(value, int):
            value = [value]
        assert len(name) == len(value)
        for valpos in xrange(len(value)):
            pos = Point._char_positions.get(name[valpos], -1)
            if pos == -1:
                raise AttributeError()
            self.array[pos] = value[valpos]

    def approx_equal(self, otherpoint, pos_tol=.01, rad_tol=.01):
        for i in xrange(0, 3):
            if math.fabs(otherpoint.array[i] - self.array[i]) > pos_tol:
                return False
            if mathutils.angle_wrap(math.radians(math.fabs(otherpoint.array[i+3] - self.array[i+3]))) > rad_tol:
                return False
        return True

    @property
    def transform(self):
        if self._transform is None:
            self._transform = mathutils.rpy_to_mat4(self.RPY).transpose().dot(mathutils.pos_to_mat4(-self.xyz))
        return self._transform

    @property
    def inverse_transform(self):
        if self._inverse_transform is None:
            self._inverse_transform = mathutils.pos_to_mat4(self.xyz).dot(mathutils.rpy_to_mat4(self.RPY))
        return self._inverse_transform

    def __str__(self):
        return "[%f,%f,%f,%f,%f,%f]" % tuple(self.array)

    def __repr__(self):
        return "Point(" + str(self) + ")"

def make_waypoint(x=0, y=0, z=0, R=0, P=0, Y=0,
                  velx=0, vely=0, velz=0, velR=0, velP=0, velY=0,
                  spdx=0, spdy=0, spdz=0, spdR=0, spdP=0, spdY=0):
    return Waypoint(Point([x, y, z, R, P, Y]),
                    Point([velx, vely, velz, velR, velP, velY]),
                    Point([spdx, spdy, spdz, spdR, spdP, spdY]))

class Waypoint(object):
    def __init__(self, pos, vel=None, speed=None):
        self.pos = pos
        self.vel = vel if vel is not None else make_point()
        self.speed = speed if speed is not None else make_point()

    def resolve_relative(self, basepoint):
        if isinstance(basepoint, Waypoint):
            basepoint = basepoint.pos
        Tinv = basepoint.inverse_transform

        pos = numpy.empty(6)
        pos[0:3] = mathutils.from_homog(Tinv.dot(mathutils.to_homog(self.pos.xyz)))
        pos[3:6] = mathutils.angle_wrap_vec(basepoint.RPY + self.pos.RPY) # TODO wrong
        vel = numpy.empty(6)
        vel[0:3] = mathutils.from_homog(Tinv.dot(mathutils.to_homog(self.vel.xyz, 0)))
        vel[3:6] = self.vel.RPY
        speed = numpy.empty(6)
        speed[0:3] = mathutils.from_homog(Tinv.dot(mathutils.to_homog(self.speed.xyz, 0)))
        speed[3:6] = self.speed.RPY
        return Waypoint(Point(pos), Point(vel), Point(speed))

    def relative_from(self, basepoint):
        if isinstance(basepoint, Waypoint):
            basepoint = basepoint.pos
        T = basepoint.transform

        relpos = numpy.empty(6)
        relpos[0:3] = mathutils.from_homog(T.dot(mathutils.to_homog(self.xyz)))
        relpos[3:6] = mathutils.angle_wrap_vec(-basepoint.RPY + self.RPY) # TODO wrong
        relvel = numpy.empty(6)
        relvel[0:3] = mathutils.from_homog(T.dot(mathutils.to_homog(self.vel.xyz, 0)))
        relvel[3:6] = self.vel.RPY
        relspeed = numpy.empty(6)
        relspeed[0:3] = mathutils.from_homog(T.dot(mathutils.to_homog(self.speed.xyz, 0)))
        relspeed[3:6] = self.speed.RPY
        return Waypoint(Point(relpos), Point(relvel), Point(speed))

    def __str__(self):
        return str(self.pos) + " @ " + str(self.vel)

    def __repr__(self):
        return "Waypoint(" + repr(self.pos) + ", " + repr(self.vel) + ")"

def set_waypoint(waypoint):
    topic = topics.get('Waypoint')
    topic.send({'r': list(waypoint.pos.xyzRPY),
                'rdot': list(waypoint.vel.xyzRPY),
                'speed': list(waypoint.speed.xyzRPY),
                'coordinate_unaligned': True})

def set_waypoint_rel(relpoint):
    set_waypoint(relpoint.resolve_relative(Waypoint(get_trajectory().pos)))

def get_waypoint():
    topic = topics.get('Waypoint')
    try:
        msg = topic.read()
        return Waypoint(Point(msg['r']), Point(msg['rdot']), Point(msg['speed']))
    except dds.Error:
        raise RuntimeError('No waypoint set')

def get_trajectory():
    topic = topics.get('Trajectory')
    try:
        return Waypoint(Point(topic.read()['xd']), Point(topic.read()['xd_dot']))
    except dds.Error:
        raise RuntimeError('No current trajectory')

def wait():
    while True:
        waypoint = get_waypoint()
        assert linalg.norm(waypoint.vel.xyzRPY) < 0.00001
        trajectory = get_trajectory()

        if trajectory.pos.approx_equal(waypoint.pos):
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
def fd(dist, spd=0):
    set_waypoint_rel(make_waypoint(x=dist, spdx=spd))

@waitopts
def bk(dist, spd=0):
    fd(-dist, spd=spd)

@waitopts
def rstrafe(dist, spd=0):
    set_waypoint_rel(make_waypoint(y=dist, spdy=spd))

@waitopts
def lstrafe(dist, spd=0):
    rstrafe(-dist, spd=spd)

@waitopts
def down(dist, spd=0):
    set_waypoint_rel(make_waypoint(z=dist, spdz=spd))

@waitopts
def up(dist, spd=0):
    down(-dist, spd=spd)

@waitopts
def rturn(deg, spd=0):
    set_waypoint_rel(make_waypoint(Y=math.radians(deg), spdY=spd))

@waitopts
def lturn(deg, spd):
    rturn(-deg, spd=spd)

@waitopts
def depth(depth, spd=0):
    waypoint = Waypoint(get_trajectory().pos)
    waypoint.pos.z = depth
    waypoint.speed.z = spd
    set_waypoint(waypoint)

@waitopts
def heading(deg=None, rad=None):
    assert(deg is not None or rad is not None)
    waypoint = Waypoint(get_trajectory().pos)
    if deg is not None:
        waypoint.pos.Y = math.radians(deg)
    else:
        waypoint.pos.Y = rad
    set_waypoint(waypoint)

def vel(x=0, y=0, z=0):
    set_waypoint_rel(make_waypoint(velx=x, vely=y, velz=z))

@waitopts
def stop():
    set_waypoint_rel(make_waypoint())

def xyz_to_waypoint(x, y, z=None, rel=False, base=None):
    curpoint = get_trajectory()
    waypoint = make_waypoint(x, y)
    if rel:
        base = curpoint
    if base is not None:
        waypoint = waypoint.resolve_relative(base)
    elif z is not None:
        waypoint.pos.z = z
    else:
        waypoint.pos.z = curpoint.pos.z
    waypoint.pos.RPY = curpoint.pos.RPY
    return waypoint

@waitopts
def go(*args, **kwargs):
    waypoint = xyz_to_waypoint(*args, **kwargs)
    set_waypoint(waypoint)

def point_shoot(*args, **kwargs):
    waypoint = xyz_to_waypoint(*args, **kwargs)
    curpoint = Waypoint(get_trajectory().pos)
    waypoint.pos.Y = math.atan2(waypoint.pos.y - curpoint.pos.y, waypoint.pos.x - curpoint.pos.x)
    heading(rad=waypoint.pos.Y)
    set_waypoint(waypoint)
    wait()

def go_seq(points, rel=False, autoyaw=True, base=None):
    if rel:
        base = Waypoint(get_trajectory().pos)
    for point in points:
        go(*point, base=base, autoyaw=autoyaw)
