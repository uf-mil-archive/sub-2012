import ctypes
import os
import sys
import weakref
import math

_ddscore_lib = ctypes.CDLL('libnddscore.so', ctypes.RTLD_GLOBAL)
_ddsc_lib = ctypes.CDLL('libnddsc.so', ctypes.RTLD_GLOBAL)

# Error checkers

class Error(Exception):
    pass

def check_code(result, func, arguments):
    if result != 0:
        raise Error({
            1: 'error',
            2: 'unsupported',
            3: 'bad parameter',
            4: 'precondition not met',
            5: 'out of resources',
            6: 'not enabled',
            7: 'immutable policy',
            8: 'inconsistant policy',
            9: 'already deleted',
            10: 'timeout',
            11: 'no data',
            12: 'illegal operation',
        }[result])

def check_null(result, func, arguments):
    if not result:
        raise Error()
    return result

def check_ex(result, func, arguments):
    if arguments[-1]._obj.value != 0:
        raise Error({
            1: '(user)',
            2: '(system)',
            3: 'bad param (system)',
            4: 'no memory (system)',
            5: 'bad typecode (system)',
            6: 'badkind (user)',
            7: 'bounds (user)',
            8: 'immutable typecode (system)',
            9: 'bad member name (user)',
            10: 'bad member id (user)',
        }[arguments[-1]._obj.value])
    return result

def check_true(result, func, arguments):
    if not result:
        raise Error()

# some types
enum = ctypes.c_int

DDS_Char = ctypes.c_char
DDS_Wchar = ctypes.c_uint32
DDS_Octet = ctypes.c_uint8
DDS_Short = ctypes.c_int16
DDS_UnsignedShort = ctypes.c_uint16
DDS_Long = ctypes.c_int32
DDS_UnsignedLong = ctypes.c_uint32
DDS_LongLong = ctypes.c_int64
DDS_UnsignedLongLong = ctypes.c_uint64
DDS_Float = ctypes.c_float
DDS_Double = ctypes.c_double
DDS_LongDouble = ctypes.c_longdouble
DDS_Boolean = ctypes.c_uint8
DDS_Enum = ctypes.c_uint32

DDS_DynamicDataMemberId = DDS_Long
DDS_ReturnCode_t = enum
DDS_ExceptionCode_t = enum
def ex():
    return ctypes.byref(DDS_ExceptionCode_t())
DDS_DomainId_t = ctypes.c_int32
DDS_TCKind = enum

DDS_SampleStateMask = DDS_UnsignedLong
DDS_ViewStateMask = DDS_UnsignedLong
DDS_InstanceStateMask = DDS_UnsignedLong
DDS_StatusMask = DDS_UnsignedLong

DDS_NOT_READ_SAMPLE_STATE = 1 << 1
DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED = 0

class TCKind(object):
    NULL = 0
    SHORT = 1
    LONG = 2
    USHORT = 3
    ULONG = 4
    FLOAT = 5
    DOUBLE = 6
    BOOLEAN = 7
    CHAR = 8
    OCTET = 9
    STRUCT = 10
    UNION = 11
    ENUM = 12
    STRING = 13
    SEQUENCE = 14
    ARRAY = 15
    ALIAS = 16
    LONGLONG = 17
    ULONGLONG = 18
    LONGDOUBLE = 19
    WCHAR = 20
    WSTRING = 21
    VALUE = 22
    SPARSE = 23
    RAW_BYTES = 0x7e
    RAW_BYTES_KEYED = 0x7f

# Function and structure accessors

def get(name, type):
    return ctypes.cast(getattr(_ddsc_lib, 'DDS_' + name), ctypes.POINTER(type)).contents

@apply
class DDSFunc(object):
    pass

@apply
class DDSType(object):
    def __getattr__(self, attr):
        contents = type(attr, (ctypes.Structure,), {})

        def g(self2, attr2):
            f = getattr(DDSFunc, attr + '_' + attr2)
            def m(*args):
                return f(self2, *args)
            setattr(self2, attr2, m)
            return m
        # make structs dynamically present bound methods
        contents.__getattr__ = g

        # take advantage of POINTERs being cached to make type pointers do the same
        ctypes.POINTER(contents).__getattr__ = g

        setattr(self, attr, contents)
        return contents

DDSType.Topic._fields_ = [
    ('_as_Entity', ctypes.c_void_p),
    ('_as_TopicDescription', ctypes.POINTER(DDSType.TopicDescription)),
]
ctypes.POINTER(DDSType.Topic).as_topicdescription = lambda self: self.contents._as_TopicDescription

DDSType.DynamicDataSeq._fields_ = DDSType.SampleInfoSeq._fields_ = DDSType.OctetSeq._fields_ = DDSType.ConditionSeq._fields_ = [
    ('_owned', DDS_Boolean),
    ('_contiguous_buffer', ctypes.c_void_p),
    ('_discontiguous_buffer', ctypes.POINTER(ctypes.c_void_p)),
    ('_maximum', DDS_UnsignedLong),
    ('_length', DDS_UnsignedLong),
    ('_sequence_init', DDS_Long),
    ('_read_token1', ctypes.c_void_p),
    ('_read_token2', ctypes.c_void_p),
    ('_elementPointersAllocation', DDS_Boolean),
]

DDSType.InstanceHandle_t._fields_ = [
    ('keyHash_value', ctypes.c_byte * 16),
    ('keyHash_length', ctypes.c_uint32),
    ('isValid', ctypes.c_int),
]
DDS_HANDLE_NIL = DDSType.InstanceHandle_t((ctypes.c_byte * 16)(*[0]*16), 16, False)

DDSType.Duration_t._fields_ = DDSType.Time_t._fields_ = [
    ('sec', DDS_Long),
    ('nanosec', DDS_UnsignedLong),
]

DDSType.SequenceNumber_t._fields_ = [
    ('high', DDS_Long),
    ('low', DDS_UnsignedLong),
]

DDSType.GUID_t._fields_ = [
    ('value', DDS_Octet * 16)
]

class InstanceState:
    ALIVE = (1 << 0)
    DISPOSED = (1 << 1)
    NO_WRITERS = (1 << 2)

DDSType.SampleInfo._fields_ = [
    ('sample_state', DDS_Enum),
    ('view_state', DDS_Enum),
    ('instance_state', DDS_Enum),
    ('source_timestamp', DDSType.Time_t),
    ('instance_handle', DDSType.InstanceHandle_t),
    ('publication_handle', DDSType.InstanceHandle_t),
    ('disposed_generation_count', DDS_Long),
    ('no_writers_generation_count', DDS_Long),
    ('sample_rank', DDS_Long),
    ('generation_rank', DDS_Long),
    ('absolute_generation_rank', DDS_Long),
    ('valid_data', DDS_Boolean),
    ('reception_timestamp', DDSType.Time_t),
    ('publication_sequence_number', DDSType.SequenceNumber_t),
    ('reception_sequence_number', DDSType.SequenceNumber_t),
    ('publication_virtual_guid', DDSType.GUID_t),
    ('publication_virtual_sequence_number', DDSType.SequenceNumber_t),
    ('original_publication_virtual_guid', DDSType.GUID_t),
    ('original_publication_virtual_sequence_number', DDSType.SequenceNumber_t),
]

def duration(t):
    d = DDSType.Duration_t()
    (frac, sec) = math.modf(t)
    d.sec = int(sec)
    d.nanosec = int(frac * 1e9)
    return d

# some types
enum = ctypes.c_int

DDSType.TopicDataQosPolicy._fields_ = [
    ('value', DDSType.OctetSeq),
]

class DurabilityQosPolicyKind:
    VOLATILE = 0
    TRANSIENT_LOCAL = 1
    TRANSIENT = 2
    PERSISTENT = 3

DDSType.DurabilityQosPolicy._fields_ = [
    ('kind', DDS_Enum),
    ('direct_communication', DDS_Boolean),
]

class HistoryQosPolicyKind:
    KEEP_LAST = 0
    KEEP_ALL = 1

DDSType.DurabilityServiceQosPolicy._fields_ = [
    ('service_cleanup_delay', DDSType.Duration_t),
    ('history_kind', DDS_Enum),
    ('history_depth', DDS_Long),
    ('max_samples', DDS_Long),
    ('max_instances', DDS_Long),
    ('max_samples_per_instance', DDS_Long),
]

DDSType.DeadlineQosPolicy._fields_ = DDSType.LatencyBudgetQosPolicy._fields_ = DDSType.LifespanQosPolicy._fields_ = [
    ('period', DDSType.Duration_t),
]

class LivelinessQosPolicyKind:
    AUTOMATIC = 0
    MANUAL_BY_PARTICIPANT = 1
    MANUAL_BY_TOPIC = 2

DDSType.LivelinessQosPolicy._fields_ = [
    ('kind', DDS_Enum),
    ('lease_duration', DDSType.Duration_t),
]

class ReliabilityQosPolicyKind:
    BEST_EFFORT = 0
    RELIABLE = 1

DDSType.ReliabilityQosPolicy._fields_ = [
    ('kind', DDS_Enum),
    ('max_blocking_time', DDSType.Duration_t),
]

class DestinationOrderQosPolicyKind:
    BY_RECEPTION_TIMESTAMP = 0
    BY_SOURCE_TIMESTAMP = 1

DDSType.DestinationOrderQosPolicy._fields_ = [
    ('kind', DDS_Enum),
    ('undocumented_field_that_wasted_four_hours', DDS_Enum),
    ('source_timestamp_tolerance', DDSType.Duration_t),
]

class HistoryQosPolicy:
    KEEP_LAST = 0
    KEEP_ALL = 1

class RefilterQosPolicyKind:
    NONE = 0
    ALL = 1
    ON_DEMAND = 2

DDSType.HistoryQosPolicy._fields_ = [
    ('kind', DDS_Enum),
    ('depth', DDS_Long),
    ('refilter', DDS_Enum),
]

DDSType.ResourceLimitsQosPolicy._fields_ = [
    ('max_samples', DDS_Long),
    ('max_instances', DDS_Long),
    ('max_samples_per_instance', DDS_Long),
    ('initial_samples', DDS_Long),
    ('initial_instances', DDS_Long),
    ('instance_has_buckets', DDS_Long),
]

DDSType.TransportPriorityQosPolicy._fields_ = [
    ('value', DDS_Long),
]

class OwnershipQosPolicyKind:
    SHARED = 0
    EXCLUSIVE = 1

DDSType.OwnershipQosPolicy._fields_ = [
    ('kind', DDS_Enum),
]

DDSType.TopicProtocolQosPolicy._fields_ = [
    ('vendor_specific_entity', DDS_Boolean),
]

DDSType.TopicQos._fields_ = [
    ('topic_data', DDSType.TopicDataQosPolicy),
    ('durability', DDSType.DurabilityQosPolicy),
    ('durability_service', DDSType.DurabilityServiceQosPolicy),
    ('deadline', DDSType.DeadlineQosPolicy),
    ('latency_budget', DDSType.LatencyBudgetQosPolicy),
    ('liveliness', DDSType.LivelinessQosPolicy),
    ('reliability', DDSType.ReliabilityQosPolicy),
    ('destination_order', DDSType.DestinationOrderQosPolicy),
    ('history', DDSType.HistoryQosPolicy),
    ('resource_limits', DDSType.ResourceLimitsQosPolicy),
    ('transport_priority', DDSType.TransportPriorityQosPolicy),
    ('lifespan', DDSType.LifespanQosPolicy),
    ('ownership', DDSType.OwnershipQosPolicy),
    ('protocol', DDSType.TopicProtocolQosPolicy),
]

DDSType.Listener._fields_ = [
    ('listener_data', ctypes.c_void_p),
]

DDSType.DataReaderListener._fields_ = [
    ('as_listener', DDSType.Listener),
    ('on_requested_deadline_missed', ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.POINTER(DDSType.DataReader), ctypes.POINTER(DDSType.RequestedDeadlineMissedStatus))),
    ('on_requested_incompatible_qos', ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.POINTER(DDSType.DataReader), ctypes.POINTER(DDSType.RequestedIncompatibleQosStatus))),
    ('on_sample_rejected', ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.POINTER(DDSType.DataReader), ctypes.POINTER(DDSType.SampleRejectedStatus))),
    ('on_liveliness_changed', ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.POINTER(DDSType.DataReader), ctypes.POINTER(DDSType.LivelinessChangedStatus))),
    ('on_data_available', ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.POINTER(DDSType.DataReader))),
    ('on_subscription_matched', ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.POINTER(DDSType.DataReader), ctypes.POINTER(DDSType.SubscriptionMatchedStatus))),
    ('on_sample_lost', ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.POINTER(DDSType.DataReader), ctypes.POINTER(DDSType.SampleLostStatus))),
]

DATA_AVAILABLE_STATUS = 0x0001 << 10

ctypes.POINTER(DDSType.DataReader).as_entity = lambda self: ctypes.cast(self, ctypes.POINTER(DDSType.Entity))
ctypes.POINTER(DDSType.StatusCondition).as_condition = lambda self: ctypes.cast(self, ctypes.POINTER(DDSType.Condition))
ctypes.POINTER(DDSType.ReadCondition).as_condition = lambda self: ctypes.cast(self, ctypes.POINTER(DDSType.Condition))
ctypes.POINTER(DDSType.Condition).__hash__ = lambda self: hash(ctypes.addressof(self.contents))

# Function prototypes

_dyn_basic_types = {
    TCKind.LONG: ('long', DDS_Long, (-2**31, 2**31)),
    TCKind.ULONG: ('ulong', DDS_UnsignedLong, (0, 2**32)),
    TCKind.SHORT: ('short', DDS_Short, (-2**15, 2**15)),
    TCKind.USHORT: ('ushort', DDS_UnsignedShort, (0, 2**16)),
    TCKind.LONGLONG: ('longlong', DDS_LongLong, (-2**63, 2**63)),
    TCKind.ULONGLONG: ('ulonglong', DDS_UnsignedLongLong, (0, 2**64)),
    TCKind.FLOAT: ('float', DDS_Float, None),
    TCKind.DOUBLE: ('double', DDS_Double, None),
    TCKind.BOOLEAN: ('boolean', DDS_Boolean, None),
    TCKind.OCTET: ('octet', DDS_Octet, (0, 2**8)),
    TCKind.CHAR: ('char', DDS_Char, None),
    TCKind.WCHAR: ('wchar', DDS_Wchar, None),
}
def _define_func((p, errcheck, restype, argtypes)):
    f = getattr(_ddsc_lib, 'DDS_' + p)
    if errcheck is not None:
        f.errcheck = errcheck
    f.restype = restype
    f.argtypes = argtypes
    setattr(DDSFunc, p, f)
map(_define_func, [
    ('DomainParticipantFactory_get_instance', check_null, ctypes.POINTER(DDSType.DomainParticipantFactory), []),
    ('DomainParticipantFactory_create_participant', check_null, ctypes.POINTER(DDSType.DomainParticipant), [ctypes.POINTER(DDSType.DomainParticipantFactory), DDS_DomainId_t, ctypes.POINTER(DDSType.DomainParticipantQos), ctypes.POINTER(DDSType.DomainParticipantListener), DDS_StatusMask]),
    ('DomainParticipantFactory_delete_participant', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DomainParticipantFactory), ctypes.POINTER(DDSType.DomainParticipant)]),

    ('DomainParticipant_create_publisher', check_null, ctypes.POINTER(DDSType.Publisher), [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.PublisherQos), ctypes.POINTER(DDSType.PublisherListener), DDS_StatusMask]),
    ('DomainParticipant_delete_publisher', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.Publisher)]),
    ('DomainParticipant_create_subscriber', check_null, ctypes.POINTER(DDSType.Subscriber), [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.SubscriberQos), ctypes.POINTER(DDSType.SubscriberListener), DDS_StatusMask]),
    ('DomainParticipant_delete_subscriber', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.Subscriber)]),
    ('DomainParticipant_create_topic', check_null, ctypes.POINTER(DDSType.Topic), [ctypes.POINTER(DDSType.DomainParticipant), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(DDSType.TopicQos), ctypes.POINTER(DDSType.TopicListener), DDS_StatusMask]),
    ('DomainParticipant_delete_topic', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.Topic)]),
    ('DomainParticipant_get_default_topic_qos', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.TopicQos)]),
    ('DomainParticipant_create_datawriter', check_null, ctypes.POINTER(DDSType.DataWriter), [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.Topic), ctypes.POINTER(DDSType.DataWriterQos), ctypes.POINTER(DDSType.DataWriterListener), DDS_StatusMask]),
    ('DomainParticipant_delete_datawriter', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.DataWriter)]),
    ('DomainParticipant_create_datareader', check_null, ctypes.POINTER(DDSType.DataReader), [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.TopicDescription), ctypes.POINTER(DDSType.DataReaderQos), ctypes.POINTER(DDSType.DataReaderListener), DDS_StatusMask]),
    ('DomainParticipant_delete_datareader', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DomainParticipant), ctypes.POINTER(DDSType.DataReader)]),

    ('Publisher_create_datawriter', check_null, ctypes.POINTER(DDSType.DataWriter), [ctypes.POINTER(DDSType.Publisher), ctypes.POINTER(DDSType.Topic), ctypes.POINTER(DDSType.DataWriterQos), ctypes.POINTER(DDSType.DataWriterListener), DDS_StatusMask]),
    ('Publisher_delete_datawriter', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.Publisher), ctypes.POINTER(DDSType.DataWriter)]),

    ('Subscriber_create_datareader', check_null, ctypes.POINTER(DDSType.DataReader), [ctypes.POINTER(DDSType.Subscriber), ctypes.POINTER(DDSType.TopicDescription), ctypes.POINTER(DDSType.DataReaderQos), ctypes.POINTER(DDSType.DataReaderListener), DDS_StatusMask]),
    ('Subscriber_delete_datareader', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.Subscriber), ctypes.POINTER(DDSType.DataReader)]),

    ('DataReader_set_listener', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DataReader), ctypes.POINTER(DDSType.DataReaderListener), DDS_StatusMask]),
    ('DataReader_create_readcondition', check_null, ctypes.POINTER(DDSType.ReadCondition), [ctypes.POINTER(DDSType.DataReader), DDS_UnsignedLong, DDS_UnsignedLong, DDS_UnsignedLong]),
    ('DataReader_delete_readcondition', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DataReader), ctypes.POINTER(DDSType.ReadCondition)]),

    ('DynamicDataTypeSupport_new', check_null, ctypes.POINTER(DDSType.DynamicDataTypeSupport), [ctypes.POINTER(DDSType.TypeCode), ctypes.POINTER(DDSType.DynamicDataTypeProperty_t)]),
    ('DynamicDataTypeSupport_delete', None, None, [ctypes.POINTER(DDSType.DynamicDataTypeSupport)]),
    ('DynamicDataTypeSupport_register_type', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicDataTypeSupport), ctypes.POINTER(DDSType.DomainParticipant), ctypes.c_char_p]),
    ('DynamicDataTypeSupport_unregister_type', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicDataTypeSupport), ctypes.POINTER(DDSType.DomainParticipant), ctypes.c_char_p]),
    ('DynamicDataTypeSupport_create_data', check_null, ctypes.POINTER(DDSType.DynamicData), [ctypes.POINTER(DDSType.DynamicDataTypeSupport)]),
    ('DynamicDataTypeSupport_delete_data', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicDataTypeSupport), ctypes.POINTER(DDSType.DynamicData)]),
    ('DynamicDataTypeSupport_print_data', None, None, [ctypes.POINTER(DDSType.DynamicDataTypeSupport), ctypes.POINTER(DDSType.DynamicData)]),

    ('DynamicData_new', check_null, ctypes.POINTER(DDSType.DynamicData), [ctypes.POINTER(DDSType.TypeCode), ctypes.POINTER(DDSType.DynamicDataProperty_t)]),
] + [
    ('DynamicData_get_' + func_name, check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.POINTER(data_type), ctypes.c_char_p, DDS_DynamicDataMemberId])
        for func_name, data_type, bounds in _dyn_basic_types.itervalues()
] + [
    ('DynamicData_set_' + func_name, check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.c_char_p, DDS_DynamicDataMemberId, data_type])
        for func_name, data_type, bounds  in _dyn_basic_types.itervalues()
] + [
    ('DynamicData_get_string', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(DDS_UnsignedLong), ctypes.c_char_p, DDS_DynamicDataMemberId]),
    ('DynamicData_get_wstring', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.POINTER(ctypes.c_wchar_p), ctypes.POINTER(DDS_UnsignedLong), ctypes.c_char_p, DDS_DynamicDataMemberId]),
    ('DynamicData_get_char_array', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.POINTER(DDS_Char), ctypes.POINTER(DDS_UnsignedLong), ctypes.c_char_p, DDS_DynamicDataMemberId]),
    ('DynamicData_get_octet_array', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.POINTER(DDS_Octet), ctypes.POINTER(DDS_UnsignedLong), ctypes.c_char_p, DDS_DynamicDataMemberId]),
    ('DynamicData_set_string', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.c_char_p, DDS_DynamicDataMemberId, ctypes.c_char_p]),
    ('DynamicData_set_wstring', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.c_char_p, DDS_DynamicDataMemberId, ctypes.c_wchar_p]),
    ('DynamicData_bind_complex_member', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.POINTER(DDSType.DynamicData), ctypes.c_char_p, DDS_DynamicDataMemberId]),
    ('DynamicData_unbind_complex_member', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.POINTER(DDSType.DynamicData)]),
    ('DynamicData_get_member_type', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicData), ctypes.POINTER(ctypes.POINTER(DDSType.TypeCode)), ctypes.c_char_p, DDS_DynamicDataMemberId]),
    ('DynamicData_get_member_count', None, DDS_UnsignedLong, [ctypes.POINTER(DDSType.DynamicData)]),
    ('DynamicData_get_type', check_null, ctypes.POINTER(DDSType.TypeCode), [ctypes.POINTER(DDSType.DynamicData)]),
    ('DynamicData_get_type_kind', None, DDS_TCKind, [ctypes.POINTER(DDSType.DynamicData)]),
    ('DynamicData_delete', None, None, [ctypes.POINTER(DDSType.DynamicData)]),

    ('DynamicDataWriter_narrow', check_null, ctypes.POINTER(DDSType.DynamicDataWriter), [ctypes.POINTER(DDSType.DataWriter)]),
    ('DynamicDataWriter_as_datawriter', check_null, ctypes.POINTER(DDSType.DataWriter), [ctypes.POINTER(DDSType.DynamicDataWriter)]),
    ('DynamicDataWriter_write', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicDataWriter), ctypes.POINTER(DDSType.DynamicData), ctypes.POINTER(DDSType.InstanceHandle_t)]),

    ('DynamicDataReader_narrow', check_null, ctypes.POINTER(DDSType.DynamicDataReader), [ctypes.POINTER(DDSType.DataReader)]),
    ('DynamicDataReader_as_datareader', check_null, ctypes.POINTER(DDSType.DataReader), [ctypes.POINTER(DDSType.DynamicDataReader)]),
    ('DynamicDataReader_take', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicDataReader), ctypes.POINTER(DDSType.DynamicDataSeq), ctypes.POINTER(DDSType.SampleInfoSeq), DDS_Long, DDS_SampleStateMask, DDS_ViewStateMask, DDS_InstanceStateMask]),
    ('DynamicDataReader_read', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicDataReader), ctypes.POINTER(DDSType.DynamicDataSeq), ctypes.POINTER(DDSType.SampleInfoSeq), DDS_Long, DDS_SampleStateMask, DDS_ViewStateMask, DDS_InstanceStateMask]),
    ('DynamicDataReader_return_loan', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.DynamicDataReader), ctypes.POINTER(DDSType.DynamicDataSeq), ctypes.POINTER(DDSType.SampleInfoSeq)]),

    ('TypeCode_name', check_ex, ctypes.c_char_p, [ctypes.POINTER(DDSType.TypeCode), ctypes.POINTER(DDS_ExceptionCode_t)]),
    ('TypeCode_kind', check_ex, DDS_TCKind, [ctypes.POINTER(DDSType.TypeCode), ctypes.POINTER(DDS_ExceptionCode_t)]),
    ('TypeCode_member_count', check_ex, DDS_UnsignedLong, [ctypes.POINTER(DDSType.TypeCode), ctypes.POINTER(DDS_ExceptionCode_t)]),
    ('TypeCode_member_name', check_ex, ctypes.c_char_p, [ctypes.POINTER(DDSType.TypeCode), DDS_UnsignedLong, ctypes.POINTER(DDS_ExceptionCode_t)]),
    ('TypeCode_member_type', check_ex, ctypes.POINTER(DDSType.TypeCode), [ctypes.POINTER(DDSType.TypeCode), DDS_UnsignedLong, ctypes.POINTER(DDS_ExceptionCode_t)]),
    ('TypeCode_member_ordinal', check_ex, DDS_UnsignedLong, [ctypes.POINTER(DDSType.TypeCode), DDS_UnsignedLong, ctypes.POINTER(DDS_ExceptionCode_t)]),

    ('DynamicDataSeq_initialize', check_true, DDS_Boolean, [ctypes.POINTER(DDSType.DynamicDataSeq)]),
    ('DynamicDataSeq_get_length', None, DDS_Long, [ctypes.POINTER(DDSType.DynamicDataSeq)]),
    ('DynamicDataSeq_get_reference', check_null, ctypes.POINTER(DDSType.DynamicData), [ctypes.POINTER(DDSType.DynamicDataSeq), DDS_Long]),

    ('SampleInfoSeq_initialize', check_true, DDS_Boolean, [ctypes.POINTER(DDSType.SampleInfoSeq)]),
    ('SampleInfoSeq_get_length', None, DDS_Long, [ctypes.POINTER(DDSType.SampleInfoSeq)]),
    ('SampleInfoSeq_get_reference', check_null, ctypes.POINTER(DDSType.SampleInfo), [ctypes.POINTER(DDSType.SampleInfoSeq), DDS_Long]),

    ('ConditionSeq_initialize', check_true, DDS_Boolean, [ctypes.POINTER(DDSType.ConditionSeq)]),
    ('ConditionSeq_get_length', None, DDS_Long, [ctypes.POINTER(DDSType.ConditionSeq)]),
    ('ConditionSeq_get_reference', check_null, ctypes.POINTER(DDSType.Condition), [ctypes.POINTER(DDSType.ConditionSeq), DDS_Long]),

    ('Entity_get_statuscondition', check_null, ctypes.POINTER(DDSType.StatusCondition), [ctypes.POINTER(DDSType.Entity)]),
    ('StatusCondition_set_enabled_statuses', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.StatusCondition)]),
    ('Condition_get_trigger_value', None, DDS_Boolean, [ctypes.POINTER(DDSType.Condition)]),

    ('WaitSet_new', check_null, ctypes.POINTER(DDSType.WaitSet), []),
    ('WaitSet_attach_condition', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.WaitSet), ctypes.POINTER(DDSType.Condition)]),
    ('WaitSet_detach_condition', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.WaitSet), ctypes.POINTER(DDSType.Condition)]),
    ('WaitSet_wait', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.WaitSet)]),
    ('WaitSet_delete', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.WaitSet)]),


    ('String_free', None, None, [ctypes.c_char_p]),
    ('Wstring_free', None, None, [ctypes.c_wchar_p]),

    ('TopicQos_initialize', check_code, DDS_ReturnCode_t, [ctypes.POINTER(DDSType.TopicQos)]),
])

def write_into_dd_member(obj, dd, member_name=None, member_id=DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED):
    tc = ctypes.POINTER(DDSType.TypeCode)()
    dd.get_member_type(ctypes.byref(tc), member_name, member_id, ex())

    kind = tc.kind(ex())
    if kind in _dyn_basic_types:
        func_name, data_type, bounds = _dyn_basic_types[kind]
        if bounds is not None and not bounds[0] <= obj < bounds[1]:
            raise ValueError('%r not in range [%r, %r)' % (obj, bounds[0], bounds[1]))
        getattr(dd, 'set_' + func_name)(member_name, member_id, obj)
    elif kind == TCKind.STRUCT or kind == TCKind.SEQUENCE or kind == TCKind.ARRAY:
        inner = DDSFunc.DynamicData_new(None, get('DYNAMIC_DATA_PROPERTY_DEFAULT', DDSType.DynamicDataProperty_t))
        try:
            dd.bind_complex_member(inner, member_name, member_id)
            try:
                write_into_dd(obj, inner)
            finally:
                dd.unbind_complex_member(inner)
        finally:
            inner.delete()
    elif kind == TCKind.STRING:
        if '\0' in obj:
            raise ValueError('strings can not contain null characters')
        dd.set_string(member_name, member_id, obj)
    elif kind == TCKind.WSTRING:
        dd.set_wstring(member_name, member_id, obj)
    elif kind == TCKind.ENUM:
        assert isinstance(obj, str)
        ordinal = None
        for i in xrange(tc.member_count(ex())):
            if obj == tc.member_name(i, ex()):
                ordinal = tc.member_ordinal(i, ex())
                break
        if ordinal is not None:
            dd.set_ulong(member_name, member_id, ordinal)
        else:
            raise ValueError('invalid enumeration string')
    else:
        raise NotImplementedError(kind)

def write_into_dd(obj, dd):
    kind = dd.get_type_kind()
    if kind == TCKind.STRUCT:
        assert isinstance(obj, dict)
        tc = dd.get_type()
        for i in xrange(tc.member_count(ex())):
            name = tc.member_name(i, ex())
            write_into_dd_member(obj[name], dd, member_name=name)
    elif kind == TCKind.ARRAY or kind == TCKind.SEQUENCE:
        assert isinstance(obj, (list, tuple))
        for i, x in enumerate(obj):
            write_into_dd_member(x, dd, member_id=i+1)
    else:
        raise NotImplementedError(kind)

def unpack_dd_member(dd, member_name=None, member_id=DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED):
    tc = ctypes.POINTER(DDSType.TypeCode)()
    dd.get_member_type(ctypes.byref(tc), member_name, member_id, ex())

    kind = tc.kind(ex())
    if kind in _dyn_basic_types:
        func_name, data_type, bounds = _dyn_basic_types[kind]
        inner = data_type()
        getattr(dd, 'get_' + func_name)(ctypes.byref(inner), member_name, member_id)
        return inner.value
    elif kind == TCKind.STRUCT or kind == TCKind.SEQUENCE or kind == TCKind.ARRAY:
        inner = DDSFunc.DynamicData_new(None, get('DYNAMIC_DATA_PROPERTY_DEFAULT', DDSType.DynamicDataProperty_t))
        try:
            dd.bind_complex_member(inner, member_name, member_id)
            try:
                # special case character/octet arrays into strings
                if kind == TCKind.SEQUENCE or kind == TCKind.ARRAY:
                    child_tc = ctypes.POINTER(DDSType.TypeCode)()
                    inner.get_member_type(ctypes.byref(child_tc), None, 1, ex()) # seems to work even for empty strings, though we're requesting the first member's type
                    child_tk = child_tc.kind(ex())
                    if child_tk in [TCKind.OCTET, TCKind.CHAR]:
                        res = ctypes.create_string_buffer(inner.get_member_count())
                        res_p = ctypes.cast(res, ctypes.POINTER(_dyn_basic_types[child_tk][1]))
                        (dd.get_octet_array if child_tk == TCKind.OCTET else dd.get_char_array)(
                            res_p, ctypes.byref(DDS_UnsignedLong(inner.get_member_count())), member_name, member_id)
                        return res.raw

                return unpack_dd(inner)
            finally:
                dd.unbind_complex_member(inner)
        finally:
            inner.delete()
    elif kind == TCKind.STRING:
        inner = ctypes.c_char_p(None)
        try:
            dd.get_string(ctypes.byref(inner), None, member_name, member_id)
            return inner.value
        finally:
            DDSFunc.String_free(inner)
    elif kind == TCKind.WSTRING:
        inner = ctypes.c_wchar_p(None)
        try:
            dd.get_wstring(ctypes.byref(inner), None, member_name, member_id)
            return inner.value
        finally:
            DDSFunc.Wstring_free(inner)
    elif kind == TCKind.ENUM:
        inner = DDS_UnsignedLong()
        dd.get_ulong(ctypes.byref(inner), member_name, member_id)
        for i in xrange(tc.member_count(ex())):
            if inner.value == tc.member_ordinal(i, ex()):
                return tc.member_name(i, ex())
        raise Error('bad enum ordinal %d' % inner.value)
    else:
        raise NotImplementedError(kind)

def unpack_dd(dd):
    kind = dd.get_type_kind()
    if kind == TCKind.STRUCT:
        obj = {}
        tc = dd.get_type()
        for i in xrange(tc.member_count(ex())):
            name = tc.member_name(i, ex())
            obj[name] = unpack_dd_member(dd, member_name=name)
        return obj
    elif kind == TCKind.ARRAY or kind == TCKind.SEQUENCE:
        obj = []
        for i in xrange(dd.get_member_count()):
            obj.append(unpack_dd_member(dd, member_id=i+1))
        if kind == TCKind.ARRAY:
            obj = tuple(obj)
        return obj
    else:
        raise NotImplementedError(kind)

_outside_refs = set()
_refs = set()

class WrappedObject(object):
    def __init__(self, inner, deps, free_func=lambda inner: None, _refs=_refs):
        self._inner = inner
        self._as_parameter_ = inner
        _refs.add(weakref.ref(self, lambda ref: (free_func(inner), _refs.remove(ref), deps)))


    def __getattr__(self, attr):
        return getattr(self._inner, attr)

class Topic(object):
    def __init__(self, part, name, data_type, qos=None):
        self._part = part
        self.name = name
        self.data_type = data_type
        self.qos = qos if qos is not None else part.get_default_topic_qos()

        self._support = WrappedObject(
            DDSFunc.DynamicDataTypeSupport_new(self.data_type._get_typecode(), get('DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT', DDSType.DynamicDataTypeProperty_t)),
            self.data_type,
            lambda support: (support.unregister_type(self._part._participant, data_type.name), support.delete()),
        )
        self._support.register_type(self._part._participant, self.data_type.name)

        self._topic = WrappedObject(self._part._participant.create_topic(
            self.name,
            self.data_type.name,
            self.qos,
            None,
            0,
        ), [self._part._participant, self._support], self._part._participant.delete_topic)

        self._listener = None
        self._writer = None
        self._reader = None

        self._callbacks = {}

    def _create_reader(self):
        assert self._reader is None
        self._reader = WrappedObject(self._part._participant.create_datareader(
            self._topic.as_topicdescription(),
            get('DATAREADER_QOS_USE_TOPIC_QOS', DDSType.DataReaderQos),
            None,
            0,
        ), [self._topic, self._part._participant], self._part._participant.delete_datareader)
        self._dyn_narrowed_reader = WrappedObject(DDSFunc.DynamicDataReader_narrow(self._reader), self._reader)

        self._reader_readcondition = WrappedObject(
            self._reader.create_readcondition(DDS_NOT_READ_SAMPLE_STATE, get('ANY_VIEW_STATE', DDS_ViewStateMask), InstanceState.ALIVE),
            self._reader,
            self._reader.delete_readcondition)

    def _enable_listener(self):
        assert self._listener is None
        if self._reader is None:
            self._create_reader()
        self._listener = DDSType.DataReaderListener(on_data_available=ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.POINTER(DDSType.DataReader))(self._data_available_callback))
        self._reader.set_listener(self._listener, DATA_AVAILABLE_STATUS)
        _outside_refs.add(self) # really want self._listener, but this does the same thing

    def _disable_listener(self):
        assert self._listener is not None
        self._reader.set_listener(None, 0)
        self._listener = None
        _outside_refs.remove(self)

    def add_data_available_callback(self, cb):
        '''Warning: callback is called back in another thread!'''
        if not self._callbacks:
            self._enable_listener()
        ref = max(self._callbacks) if self._callbacks else 0
        self._callbacks[ref] = cb
        return ref

    def remove_data_available_callback(self, ref):
        del self._callbacks[ref]
        if not self._callbacks:
            self._disable_listener()

    def _data_available_callback(self, listener_data, datareader):
        for cb in self._callbacks.itervalues():
            cb()

    def send(self, msg):
        if self._writer is None:
            self._writer = WrappedObject(self._part._participant.create_datawriter(
                self._topic,
                get('DATAWRITER_QOS_USE_TOPIC_QOS', DDSType.DataWriterQos),
                None,
                0,
            ), [self._topic], self._part._participant.delete_datawriter)
            self._dyn_narrowed_writer = WrappedObject(DDSFunc.DynamicDataWriter_narrow(self._writer), self._writer)

        sample = self._support.create_data()

        try:
            write_into_dd(msg, sample)
            self._dyn_narrowed_writer.write(sample, DDS_HANDLE_NIL)
        finally:
            self._support.delete_data(sample)

    def take(self):
        if self._reader is None:
            self._create_reader()

        data_seq = DDSType.DynamicDataSeq()
        data_seq.initialize()
        info_seq = DDSType.SampleInfoSeq()
        info_seq.initialize()
        self._dyn_narrowed_reader.take(ctypes.byref(data_seq), ctypes.byref(info_seq), 1, get('ANY_SAMPLE_STATE', DDS_SampleStateMask), get('ANY_VIEW_STATE', DDS_ViewStateMask), InstanceState.ALIVE)
        try:
            info = info_seq.get_reference(0)
            return unpack_dd(data_seq.get_reference(0))
        finally:
            self._dyn_narrowed_reader.return_loan(ctypes.byref(data_seq), ctypes.byref(info_seq))

    def read(self):
        if self._reader is None:
            self._create_reader()

        data_seq = DDSType.DynamicDataSeq()
        data_seq.initialize()
        info_seq = DDSType.SampleInfoSeq()
        info_seq.initialize()
        self._dyn_narrowed_reader.read(ctypes.byref(data_seq), ctypes.byref(info_seq), 1, get('ANY_SAMPLE_STATE', DDS_SampleStateMask), get('ANY_VIEW_STATE', DDS_ViewStateMask), InstanceState.ALIVE)
        try:
            info = info_seq.get_reference(0)
            return unpack_dd(data_seq.get_reference(0))
        finally:
            self._dyn_narrowed_reader.return_loan(ctypes.byref(data_seq), ctypes.byref(info_seq))

    def read_all(self):
        if self._reader is None:
            self._create_reader()

        data_seq = DDSType.DynamicDataSeq()
        data_seq.initialize()
        info_seq = DDSType.SampleInfoSeq()
        info_seq.initialize()
        self._dyn_narrowed_reader.read(ctypes.byref(data_seq), ctypes.byref(info_seq), get('LENGTH_UNLIMITED', DDS_Long), get('ANY_SAMPLE_STATE', DDS_SampleStateMask), get('ANY_VIEW_STATE', DDS_ViewStateMask), InstanceState.ALIVE)

        try:
            return [unpack_dd(data_seq.get_reference(i)) for i in xrange(data_seq.get_length())]
        finally:
            self._dyn_narrowed_reader.return_loan(ctypes.byref(data_seq), ctypes.byref(info_seq))

    def get_condition(self):
        if self._reader is None:
            self._create_reader()
        return self._reader_readcondition.as_condition()

class WaitSet(object):
    def __init__(self):
        self._waitset = WrappedObject(DDSFunc.WaitSet_new(), [], DDSFunc.WaitSet_delete)
        self._objcond = {}

    def attach(self, topic, obj):
        cond = topic.get_condition()
        self._waitset.attach_condition(cond)
        self._objcond[cond] = obj

    def delete(self):
        DDSFunc.WaitSet_delete(self._waitset)
        self._waitset = None

    def detach(self, topic):
        cond = topic.get_condition()
        self._waitset.detach_condition(cond)
        del self._objcond[cond]

    def wait(self, timeout):
        cond_seq = DDSType.ConditionSeq()
        DDSFunc.ConditionSeq_initialize(cond_seq)

        try:
            if timeout > 0:
                d = duration(timeout)
            else:
                d = get('DURATION_INFINITE', DDSType.Duration_t)
            self._waitset.wait(ctypes.byref(cond_seq), ctypes.byref(d))

            return [obj for cond, obj in self._objcond.iteritems() if cond.get_trigger_value()]
        except Error, e:
            if e.message == 'timeout':
                return []
            else:
                raise

class Participant(object):
    def __init__(self, domain_id=0):
        self._participant = WrappedObject(DDSFunc.DomainParticipantFactory_get_instance().create_participant(
            domain_id,
            get('PARTICIPANT_QOS_DEFAULT', DDSType.DomainParticipantQos),
            None,
            0,
        ), None, DDSFunc.DomainParticipantFactory_get_instance().delete_participant)

        self._open_topics = weakref.WeakValueDictionary()

    def get_default_topic_qos(self):
        qos = DDSType.TopicQos()
        qos.initialize()
        self._participant.get_default_topic_qos(ctypes.byref(qos))
        return qos

    def get_topic(self, name, data_type, qos=None):
        res = self._open_topics.get(name, None)
        if res is not None:
            if data_type != res.data_type:
                raise ValueError('get_topic called with a previous name but a different data_type')
            return res
        res = Topic(self, name, data_type, qos)
        self._open_topics[name] = res
        return res

class LibraryType(object):
    def __init__(self, lib, name):
        self._lib, self.name = lib, name
        del lib, name

        assert self._get_typecode().name(ex()) == self.name

    def _get_typecode(self):
        f = getattr(self._lib, self.name + '_get_typecode')
        f.argtypes = []
        f.restype = ctypes.POINTER(DDSType.TypeCode)
        f.errcheck = check_null
        return f()

class Library(object):
    def __init__(self, so_path):
        self._lib = ctypes.CDLL(so_path)

    def __getattr__(self, attr):
        res = LibraryType(self._lib, attr)
        setattr(self, attr, res)
        return res
