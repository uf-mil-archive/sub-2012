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

def quat_to_rpy(q):
    return numpy.array([
            math.atan2(2*(q[0]*q[1] + q[2]*q[3]), 1 - 2*(q[1]*q[1]+q[2]*q[2])),
            math.asin(2*(q[0]*q[2] - q[3]*q[1])),
            math.atan2(2*(q[0]*q[3] + q[1]*q[2]), 1 - 2*(q[2]*q[2]+q[3]*q[3]))])
