from subjugator import topics
from subjugator import sched
from subjugator import nav

import json

# IMO we should change from IDs to strings
OBJECTID_BUOY_YELLOW = 5
OBJECTID_BUOY_RED = 6
OBJECTID_BUOY_GREEN = 7
OBJECTID_PIPE = 8

def set_ids(objectids, cameraid):
    topic = topics.get('VisionSetIDs')
    topic.send({'visionids': objectids, 'cameraid': cameraid})

def wait():
    topic = topics.get('Vision')
    sched.ddswait(topic)

def get_objects(objectid):
    topic = topics.get('Vision') # TODO, multiple camera. Need to change messages and QOS to work correctly here.
    try:
        msg = topic.read()
        return [obj in map(json.loads, msg['messages']) if int(obj['objectID']) == objectid]
    except dds.Error:
        return []
