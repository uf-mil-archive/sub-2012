import math

import dds_obj

dds = dds_obj.DataObject()

# scales are explicitly set by creating min_, max_ elements
# (only by convention - the plotter just ignores lines whose names end with underscores)

def quat_to_euler(q):
    return [
        math.degrees(math.atan2(2*(q[2]*q[3]+q[0]*q[1]), 1-2*(q[1]*q[1]+q[2]*q[2]))),
        math.degrees(-math.asin(2*(q[1]*q[3]-q[0]*q[2]))),
        math.degrees(math.atan2(2*(q[1]*q[2]+q[0]*q[3]), 1-2*(q[2]*q[2]+q[3]*q[3]))),
    ]


def quat_to_euler_dict(q):
    res = array_to_dict(quat_to_euler(q), 'Roll Pitch Yaw'.split(' '), 'deg')
    res.update({'min_': (-180, 'deg'), 'max_': (180, 'deg')})
    return res

def array_to_dict(array, names, unit):
    assert len(array) == len(names)
    return dict((names[i], (array[i], unit)) for i in xrange(3))

def flatten(d):
    return dict((k + '/' + k2, v2) for k, v in d.iteritems() for k2, v2 in v.iteritems())

def subtract_dicts(a, b):
    assert set(a.keys()) == set(b.keys())
    assert all(a[k][1] == b[k][1] for k in a.keys()) # units match
    return dict((k, (a[k][0] - b[k][0], a[k][1])) for k in a.keys())

def add_border(d, (border, border_unit)):
    assert all(unit == border_unit for name, (value, unit) in d.iteritems())
    low = min(value for name, (value, unit) in d.iteritems())
    high = min(value for name, (value, unit) in d.iteritems())
    return dict(d, lowborder_=(low-border, border_unit), highborder_=(high+border, border_unit))

position = lambda: add_border(array_to_dict(dds.LPOSVSS['position_NED'], 'XYZ', 'm'), (0.01, 'm'))
velocity = lambda: add_border(array_to_dict(dds.LPOSVSS['velocity_NED'], 'XYZ', 'm/s'), (0.01, 'm/s'))
desired_position = lambda: array_to_dict(dds.Trajectory['xd'][:3], 'XYZ', 'm')
orientation = lambda: quat_to_euler_dict(dds.LPOSVSS['quaternion_NED_B'])
desired_orientation = lambda: array_to_dict([math.degrees(x) for x in dds.Trajectory['xd'][3:]], ['Roll', 'Pitch', 'Yaw'], 'deg')

graphs = [
    ('DVL velocity', lambda: array_to_dict(dds.DVL['velocity'], 'XYZ', 'm/s')),
    ('Depth', lambda: {'Z': (dds.Depth['depth'], 'm')}),
    ('IMU', lambda: flatten({
        'a': array_to_dict(dds.IMU['acceleration'], 'XYZ', 'm/s2'),
        'g': array_to_dict(dds.IMU['angular_rate'], 'XYZ', 'rad/s'),
        'm': array_to_dict(dds.IMU['mag_field'], 'XYZ', 'uT'),
    })),
    ('Position', position),
    ('Desired position', desired_position),
    ('Position and desired position', lambda: flatten({
        't': position(),
        'd': desired_position(),
    })),
] + [
    ('Position and desired position - ' + axis, lambda axis_=axis: {
        'p': position()[axis_],
        'd': desired_position()[axis_],
    })
        for axis in 'XYZ'
] + [
    ('Position minus desired position', lambda: subtract_dicts(position(), desired_position())),
    ('Position and velocity', lambda: flatten({
        'p': position(),
        'v': velocity(),
    })),
    ('Orientation', orientation),
    ('Desired orientation', desired_orientation),
    ('Position and desired position', lambda: flatten({
        't': orientation(),
        'd': desired_orientation(),
    })),
]
