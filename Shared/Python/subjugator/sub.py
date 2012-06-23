from . import topics
import dds

## Thrusters

class Thruster(object):
    def __init__(self, num):
        self.num = num

    def _get_effort(self):
        efforttopic = topics.get('PDEffort')
        try:
            return efforttopic.read()['efforts'][self.num]
        except dds.Error:
            return 0

    def _set_effort(self, effort):
        efforttopic = topics.get('PDEffort')
        try:
            efforts = list(efforttopic.read()['efforts'])
        except dds.Error:
            efforts = [0]*8
        efforts[self.num] = effort
        efforttopic.send(dict(efforts=efforts))

    effort = property(_get_effort, _set_effort)

    @property
    def current(self):
        pdstatustopic = topics.get('PDStatus')
        try:
            return pdstatustopic.read()['current'][self.num]
        except dds.Error:
            return 0

thrusters = map(Thruster, xrange(0, 8))

## Sensors

class Sensor(object):
    def __init__(self, topicname):
        self.topicname = topicname

    @property
    def available(self):
        topic = topics.get(self.topicname)
        try:
            topic.read()
            return True
        except:
            return False

    @staticmethod
    def _makeprop(field, default):
        def get(self):
            topic = topics.get(self.topicname)
            try:
                return topic.read()[field]
            except:
                return default
        return property(get)

@apply
class IMU(Sensor):
    def __init__(self):
        Sensor.__init__(self, 'IMU')

    mag = Sensor._makeprop('mag_field', (0, 0, 0))
    accel = Sensor._makeprop('acceleration', (0, 0, 0))
    angrate = Sensor._makeprop('angular_rate', (0, 0, 0))

@apply
class DepthSensor(Sensor):
    def __init__(self):
        Sensor.__init__(self, 'Depth')
    depth = Sensor._makeprop('depth', 0)

@apply
class DVL(Sensor):
    def __init__(self):
        Sensor.__init__(self, 'DVL')
    velocity = Sensor._makeprop('velocity', (0, 0, 0))

@apply
class Hydrophones(Sensor):
    def __init__(self):
        Sensor.__init__(self, 'Hydrophone')
    declination = Sensor._makeprop('declination', 0)
    heading = Sensor._makeprop('heading', 0)
    distance = Sensor._makeprop('distance', 0)
    frequency = Sensor._makeprop('frequency', 0)
    valid = Sensor._makeprop('valid', False)

# Actuators

def set_actuators(mask):
    if isinstance(mask, str):
        mask = int(mask, 2)
    topic = topics.get('PDActuator')
    topic.send(dict(actuators_mask=mask))

def get_actuator_inputs():
    try:
        topic = topics.get('PDInput')
        mask = topic.read()['input_mask']
        return ((mask & 0x01) != 0, (mask & 0x02) != 0)
    except dds.Error:
        return (False, False)
