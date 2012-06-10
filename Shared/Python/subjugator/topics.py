import dds

from collections import namedtuple

part = dds.Participant()
lib = dds.Library('libdds_c.so')

LEGACY = ['legacy']
UNRELIABLE = []
RELIABLE = ['reliable']
EXCLUSIVE = ['exclusive']
LIVELINESS = ['liveliness']
PERSISTENT = ['reliable', 'persistent']
DEEP_PERSISTENT = ['reliable', 'persistent', 'deep_history']
DEFAULT = ['reliable', 'exclusive', 'liveliness']

_TopicConf = namedtuple('TopicConf', 'type qos')

_topic_qoses = {
    'WorkerLog': _TopicConf(lib.WorkerLogMessage, DEEP_PERSISTENT),
    'WorkerState': _TopicConf(lib.WorkerStateMessage, PERSISTENT+LIVELINESS),
    'WorkerKill': _TopicConf(lib.WorkerKillMessage, PERSISTENT+LIVELINESS),
    'PDStatus': _TopicConf(lib.PDStatusMessage, DEFAULT),
    'PDEffort': _TopicConf(lib.PDEffortMessage, DEFAULT),
    'IMU': _TopicConf(lib.IMUMessage,  EXCLUSIVE+LIVELINESS),
    'DVL': _TopicConf(lib.DVLMessage,  DEFAULT),
    'Depth': _TopicConf(lib.DepthMessage, DEFAULT),
    'ControllerGains': _TopicConf(lib.ControllerGainsMessage, PERSISTENT),
    'Trajectory': _TopicConf(lib.TrajectoryMessage, PERSISTENT),
    'Waypoint': _TopicConf(lib.WaypointMessage, PERSISTENT),
    'LPOSVSS': _TopicConf(lib.LPOSVSSMessage, DEFAULT),
    'InteractionCommand': _TopicConf(lib.InteractionCommandMessage, RELIABLE),
    'InteractionStatus': _TopicConf(lib.InteractionStatusMessage, PERSISTENT+EXCLUSIVE),
    'InteractionOutput': _TopicConf(lib.InteractionOutputMessage, DEEP_PERSISTENT),
    'VisionConfig': _TopicConf(lib.VisionConfigMessage, PERSISTENT),
    'VisionDebug': _TopicConf(lib.VisionDebugMessage, UNRELIABLE),
    'VisionResults': _TopicConf(lib.VisionResultsMessage, DEFAULT),
    'VisionSetObjects': _TopicConf(lib.VisionSetObjectsMessage, DEFAULT),
}

def get(name):
    conf = _topic_qoses[name]
    return part.get_topic(name, conf.type, make_qos(conf.qos))

def make_qos(qoslist):
    qos = part.get_default_topic_qos()
    if 'legacy' not in qoslist:
        if 'reliable' in qoslist:
            qos.reliability.kind = dds.ReliabilityQosPolicyKind.RELIABLE
        if 'exclusive' in qoslist:
            qos.ownership.kind = dds.OwnershipQosPolicyKind.EXCLUSIVE
        if 'persistent' in qoslist:
            if 'deep_history' in qoslist:
                qos.history.kind = dds.HistoryQosPolicyKind.KEEP_ALL
            qos.durability.kind = dds.DurabilityQosPolicyKind.TRANSIENT_LOCAL
        if 'liveliness' in qoslist:
            qos.liveliness.lease_duration.sec = 0
            qos.liveliness.lease_duration.nanosec = long(500E6)
    return qos
