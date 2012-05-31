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

def set_waypoint(waypoint):
    topic = topics.get('SetWaypoint')
    topic.send({'timestamp': 0, 'position_ned': list(waypoint.xyz), 'rpy': list(waypoint.RPY), 'isRelative': False})

def set_waypoint_rel(relpoint):
    set_waypoint(relpoint.resolve_relative(get_waypoint()))

def get_waypoint():
    topic = topics.get('SetWaypoint')
    try:
        msg = topic.read()
        waypoint = mathutils.waypoint()
        waypoint.xyz = msg['position_ned']
        waypoint.RPY = msg['rpy']
        return waypoint
    except dds.Error:
        raise RuntimeError('No waypoint set')

def get_trajectory():
    topic = topics.get('Trajectory')
    try:
        return mathutils.Waypoint(topic.read()['xd'])
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
    set_waypoint_rel(mathutils.waypoint(x=dist))

@waitopts
def bk(dist):
    fd(-dist)

@waitopts
def rstrafe(dist):
    set_waypoint_rel(mathutils.waypoint(y=dist))

@waitopts
def lstrafe(dist):
    rstrafe(-dist)

@waitopts
def down(dist):
    set_waypoint_rel(mathutils.waypoint(z=dist))

@waitopts
def up(dist):
    down(-dist)

@waitopts
def rturn(deg):
    set_waypoint_rel(mathutils.waypoint(Y=math.radians(deg)))

@waitopts
def lturn(deg):
    rturn(-deg)

@waitopts
def depth(depth):
    waypoint = get_waypoint()
    waypoint.z = depth
    set_waypoint(waypoint)

@waitopts
def heading(heading):
    waypoint = get_waypoint()
    waypoint.Y = heading
    set_waypoint(waypoint)

@waitopts
def go(x, y, z=None, rel=False, autoyaw=True, base=None):
    curpoint = get_waypoint()
    waypoint = mathutils.waypoint(x, y)
    if rel:
        base = curpoint
    if base is not None:
        waypoint = waypoint.resolve_relative(base)
    else:
        waypoint.z = curpoint.z # z is always relative
    if z is not None:
        waypoint.z = z
    waypoint.RP = curpoint.RP
    if autoyaw:
        waypoint.Y = math.atan2(waypoint.y - curpoint.y, waypoint.x - curpoint.x)
    else:
        waypoint.Y = curpoint.Y
    set_waypoint(waypoint)

def go_seq(points, rel=False, autoyaw=True, base=None):
    if rel:
        base = get_waypoint()
    for point in points:
        go(*point, base=base, autoyaw=autoyaw)
