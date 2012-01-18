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
    'WorkerLog': _TopicConf(lib.WorkerLogMessage, DEEP_PERSISTENT)
}

def get(name):
    conf = _topic_qoses[name]
    return part.get_topic(name, conf.type, make_qos(conf.qos))

def make_qos(qoslist):
    qos = part.get_default_topic_qos()
    if 'legacy' not in qoslist:
        qos.destination_order.kind = dds.DestinationOrderQosPolicyKind.BY_SOURCE_TIMESTAMP

        if 'reliable' in qoslist:
            qos.reliability.kind = dds.ReliabilityQosPolicyKind.RELIABLE
        if 'exclusive' in qoslist:
            qos.ownership.kind = dds.OwnershipQosPolicyKind.EXCLUSIVE
        if 'persistent' in qoslist:
            if 'deep_history' in qoslist:
                qos.history.kind = dds.HistoryQosPolicyKind.KEEP_ALL
            qos.durability.kind = dds.DurabilityQosPolicyKind.TRANSIENT_LOCAL
        if 'liveliness' in qoslist:
            qos.livelines.lease_duration.sec = 0
            qos.livelines.lease_duration.nanosec = 500E6
    return qos
