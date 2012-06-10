from subjugator import topics
from subjugator import sched

import dds
import time

class Worker(object):
    def __init__(self, name):
        self.name = name
        self.set_state('standby', 'Initializing')

    def set_state(self, statecode, statemsg):
        topic = topics.get('WorkerState')
        topic.send({'worker': self.name,
                    'state': {'code': 'WORKERSTATECODE_' + statecode.upper(),
                              'msg': statemsg}})
        self.state = statecode
        self.statemsg = statemsg

    def log(self, msg, logtype='info'):
        topic = topics.get('WorkerLogMessage')
        topic.send({'worker': self.name,
                    'type': 'WORKERLOGTYPE_' + logtype.upper(),
                    'msg': msg,
                    'time': int(time.time())})
        print '[' + time.strftime('%h:%m:%s %p') + '] ' + self.name + ': ' + msg

def get_killed_by():
    topic = topics.get('WorkerKill')
    try:
        for msg in topic.read_all():
            if msg['killed']:
                return msg['name']
        return None
    except dds.Error:
        return None

class KillMonitor(sched.Task):
    def __init__(self):
        sched.Task.__init__(self, 'killmonitor')
        self.kill_callbacks = []
        self.unkill_callbacks = []

    def register_callbacks(self, kill=None, unkill=None):
        if kill is not None:
            self.kill_callbacks.append(kill)
        if unkill is not None:
            self.unkill_callbacks.append(unkill)

    def run(self):
        topic = topics.get('WorkerKill')
        prev_killedby = None
        while True:
            killedby = get_killed_by()
            if prev_killedby is None and killedby is not None:
                for callback in self.kill_callbacks:
                    callback(killedby)
            elif prev_killedby is not None and killedby is None:
                for callback in self.unkill_callbacks:
                    callback()
            prev_killedby = killedby
            sched.ddswait(topic)

killmonitor = KillMonitor()
