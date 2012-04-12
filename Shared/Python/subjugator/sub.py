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
            return pdstatustopic.read()['current']
        except dds.Error:
            return 0

thrusters = map(Thruster, xrange(0, 8))

## Sensors

@apply
class IMU(object):
    @property
    def mag(self):
        imutopic = topics.get('IMU')
        try:
            return imutopic.read()['mag_field']
        except:
            return (0, 0, 0)

@apply
class DepthSensor(object):
    @property
    def depth(self):
        depthtopic = topics.get('Depth')
        try:
            return depthtopic.read()['depth']
        except:
            return 0
