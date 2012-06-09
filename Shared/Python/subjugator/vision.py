from subjugator import topics
from subjugator import sched
from subjugator import nav

import json


def set_objects(objectnames, cameraid):
    topic = topics.get('VisionSetObjects')
    topic.send({'objectnames': objectnames, 'cameraid': cameraid})

def wait():
    topic = topics.get('VisionResults')
    sched.ddswait(topic)

def get_objects(objectname):
    topic = topics.get('VisionResults') # TODO, multiple camera. Need to change messages and QOS to work correctly here.
    try:
        msg = topic.read()
        return [obj in map(json.loads, msg['messages']) if obj['objectName'] == objectname]
    except dds.Error:
        return []
