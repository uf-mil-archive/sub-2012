import math

import dds_obj

dds = dds_obj.DataObject()

# scales are explicitly set by creating min_, max_ elements
# (only by convention - the plotter just ignores lines whose names end with underscores)

def quat_to_euler(q):
    return {
        'min_': (-180, 'deg'), 'max_': (180, 'deg'),
        'Roll': (math.degrees(math.atan2(2*(q[2]*q[3]+q[0]*q[1]), 1-2*(q[1]*q[1]+q[2]*q[2]))), 'deg'),
        'Pitch': (math.degrees(-math.asin(2*(q[1]*q[3]-q[0]*q[2]))), 'deg'),
        'Yaw': (math.degrees(math.atan2(2*(q[1]*q[2]+q[0]*q[3]), 1-2*(q[2]*q[2]+q[3]*q[3]))), 'deg'),
    }

def array_to_dict(array, names, unit):
    assert len(array) == len(names)
    return dict((names[i], (array[i], unit)) for i in xrange(3))

def flatten(d):
    return dict((k + '/' + k2, v2) for k, v in d.iteritems() for k2, v2 in v.iteritems())

def subtract_dicts(a, b):
    assert set(a.keys()) == set(b.keys())
    assert all(a[k][1] == b[k][1] for k in a.keys()) # units match
    return dict((k, (a[k][0] - b[k][0], a[k][1])) for k in a.keys())

position = lambda: array_to_dict(dds.LPOSVSS['position_NED'], 'XYZ', 'm')
velocity = lambda: array_to_dict(dds.LPOSVSS['velocity_NED'], 'XYZ', 'm/s')
desired_position = lambda: array_to_dict(dds.Trajectory['xd'][:3], 'XYZ', 'm')
orientation = lambda: quat_to_euler(dds.LPOSVSS['quaternion_NED_B'])
desired_orientation = lambda: array_to_dict([math.degrees(x) for x in dds.Trajectory['xd'][3:]], ['Roll', 'Pitch', 'Yaw'], 'deg')

graphs = [
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
    ('Position and desired position', lambda: flatten({
        't': orientation(),
        'd': desired_orientation(),
    })),
]
