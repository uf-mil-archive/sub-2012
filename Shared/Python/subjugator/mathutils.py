import numpy
import math

def angle_wrap(val):
    while val > math.pi:
        val -= 2.0*math.pi
    while val < -math.pi:
        val += 2.0*math.pi
    return val

angle_wrap_vec = numpy.vectorize(angle_wrap, otypes=[numpy.float])

def rpy_to_mat(rpy):
    cr = math.cos(rpy[0])
    sr = math.sin(rpy[0])
    cp = math.cos(rpy[1])
    sp = math.sin(rpy[1])
    cy = math.cos(rpy[2])
    sy = math.sin(rpy[2])

    R = numpy.array([
            [cy*cp, cy*sp*sr-sy*cr, cy*sp*cr+sy*sr],
            [sy*cp, sy*sp*sr+cy*cr, sy*sp*cr-cy*sr],
            [-sp,   cp*sr,          cp*cr         ]])
    return R

def rpy_to_mat4(rpy):
    R = numpy.identity(4)
    R[0:3, 0:3] = rpy_to_mat(rpy)
    return R

def pos_to_mat4(pos):
    T = numpy.identity(4)
    T[0:3, 3] = pos
    return T

def to_homog(vec, w=1):
    return numpy.hstack((vec, numpy.array([w])))

def from_homog(vec):
    return vec[0:3]

def waypoint(x=0, y=0, z=0, R=0, P=0, Y=0):
    return Waypoint([x, y, z, R, P, Y])

class Waypoint(object):
    def __init__(self, array):
        self.array = numpy.array(array, dtype=float)
        self._transform = None
        self._inverse_transform = None

    @property
    def transform(self):
        if self._transform is None:
            self._transform = rpy_to_mat4(self.RPY).transpose().dot(pos_to_mat4(-self.xyz))
        return self._transform

    @property
    def inverse_transform(self):
        if self.inv_T is None:
            self._inverse_transform = pos_to_mat4(self.xyz).dot(rpy_to_mat4(self.RPY))
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
        waypoint[0:3] = from_homog(basepoint.inverse_transform.dot(to_homog(self.xyz)))
        waypoint[3:6] = angle_wrap_vec(basepoint.RPY + self.RPY)
        return Waypoint(waypoint)

    def relative_from(self, basepoint):
        relpoint = numpy.empty(6)
        relpoint[0:3] = from_homog(basepoint.transform.dot(to_homog(self.xyz)))
        relpoint[3:6] = angle_wrap_vec(-basepoint.RPY + self.RPY)
        return Waypoint(relpoint)

    def approx_equal(self, otherpoint, pos_tol=.01, rad_tol=.01):
        for i in xrange(0, 3):
            if math.fabs(otherpoint.array[i] - self.array[i]) > pos_tol:
                return False
            if math.fabs(otherpoint.array[i+3] - self.array[i+3]) > rad_tol:
                return False
        return True

    def __str__(self):
        return "[%f,%f,%f,%f,%f,%f]" % tuple(self.array)

    def __repr__(self):
        return "Waypoint(" + str(self) + ")"
