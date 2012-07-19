from subjugator import topics
from subjugator import sched
from subjugator import sub
from subjugator import nav

import dds
import collections
import functools

# TODO: Decided to give singletons a try, turned out badly. Need to redo this
# with a top level class that connects the dots and deals with cross cutting concerns.
# Then somehow pass this to missions

class StateManager(object):
    def __init__(self):
        self.statestack = []
        self.callbacks = []

    def add_callback(self, callback):
        self.callbacks.append(callback)

    def _run_callbacks(self):
        for callback in self.callbacks:
            callback()

    def push(self, state):
        self.statestack.append(state)
        self._run_callbacks()

    def pop(self):
        state = self.statestack.pop()
        self._run_callbacks()
        return state

    @property
    def state(self):
        return '/'.join(self.statestack)

statemanager = StateManager()

class State(object):
    def __init__(self, name, *args, **kwargs):
        self.name = name
        self.args = args
        self.kwargs = kwargs

    def __call__(self, func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            self.args = args
            self.kwargs = kwargs
            with self:
                return func(*args, **kwargs)
        return wrapper

    def __enter__(self):
        statemanager.push(self.name.format(*self.args, **self.kwargs))

    def __exit__(self, exc_type, exc_val, exc_tb):
        statemanager.pop()
        return False

Mission = collections.namedtuple('Mission', 'name func timeout')

class MissionRegistry(object):
    def __init__(self):
        self.missions = {}

    def register(self, name, func, timeout=120):
        self.missions[name] = Mission(name, func, timeout)

    def get(self, name):
        return self.missions.get(name)

    def get_missions(self):
        return self.missions.itervalues()

    def names(self):
        return (name for (name, func) in self.missions)

missionregistry = MissionRegistry()

class MissionList(object):
    def __init__(self, name):
        self.name = name
        self.missions = []

    def add(self, mission, pos=None):
        if pos is None:
            pos = len(self.missions)
        self.missions.insert(pos, mission)
        return pos

    def remove(self, pos):
        if pos < len(self.missions):
            self.missions.pop(pos)

    def get_missions(self):
        return self.missions

    def run(self):
        with State(self.name):
            for mission in self.missions:
                with State(mission.name):
                    with sched.Timeout(mission.timeout) as t:
                        ok = mission.func()
                        if not ok:
                            return (False, mission)
                    if t.activated:
                        print mission.name + ' failed due to timed out'
                        return (False, mission)
            return (True, None)

class MissionListManager(object):
    def __init__(self):
        self.main_list = MissionList('main')
        self.fail_list = MissionList('fail')

    def get_lists(self):
        return [self.main_list, self.fail_list]

    def get(self, name):
        if name == 'main':
            return self.main_list
        elif name == 'fail':
            return self.fail_list
        else:
            return None

    def run(self):
        print 'Waiting for sub unkilled'
        sub.wait_unkilled()
        print 'Running main list'
        (ok, failedmission) = self.main_list.run()
        if not ok:
            print 'Running fail list'
            self.fail_list.run()

missionlistmanager = MissionListManager()

class MissionRunner(sched.Task):
    def __init__(self):
        sched.Task.__init__(self, 'MissionRunner')
        self.task = None

    @property
    def running(self):
        return self.task is not None and self.task.state != 'stopped'

    def start(self):
        if self.running:
            self.stop()
        self.task = sched.Task('Mission', missionlistmanager.run)

    def stop(self):
        if not self.running:
            return
        self.task.stop()
        self.task = None
        try:
            nav.stop()
        except RuntimeError:
            pass

    def run(self):
        self._send_initial_messages()

        cmdtopic = topics.get('MissionCommand')
        while True:
            sched.ddswait(cmdtopic)
            msg = cmdtopic.take()
            if msg['type'] == 'MISSIONCOMMANDTYPE_START':
                self.start()
            elif msg['type'] == 'MISSIONCOMMANDTYPE_STOP':
                self.stop()
            elif msg['type'] == 'MISSIONCOMMANDTYPE_ADD_MISSION':
                missionlist = missionlistmanager.get(msg['list'])
                mission = missionregistry.get(msg['mission'])
                if missionlist is not None and mission is not None:
                    missionlist.add(mission, msg['pos'] if msg['pos'] != -1 else None)
                    self._send_missionlist(missionlist)
            elif msg['type'] == 'MISSIONCOMMANDTYPE_REMOVE_MISSION':
                missionlist = missionlistmanager.get(msg['list'])
                if missionlist is not None:
                    missionlist.remove(msg['pos'])
                    self._send_missionlist(missionlist)

    def _send_initial_messages(self):
        availtopic = topics.get('AvailableMissions')
        availtopic.send(dict(missions=[mission.name for mission in missionregistry.get_missions()]))
        for missionlist in missionlistmanager.get_lists():
            self._send_missionlist(missionlist)

    def _send_missionlist(self, missionlist):
        topic = topics.get('MissionList')
        topic.send(dict(name=missionlist.name, missions=[mission.name for mission in missionlist.get_missions()]))

missionrunner = MissionRunner()

def _ddscallback():
    topic = topics.get('MissionState')
    topic.send(dict(running=True, state=statemanager.state)) # TODO determine if actually running
statemanager.add_callback(_ddscallback)

missiondata = dict() # Shared dictionary for missions to store and share data
