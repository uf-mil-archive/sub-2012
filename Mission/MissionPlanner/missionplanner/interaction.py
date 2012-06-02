from subjugator import topics
from subjugator import sched
import dds

import codeop
import traceback
import greenlet
import sys

class CommandTask(sched.Task):
    def __init__(self, cmd, env):
        sched.Task.__init__(self, 'Command')
        self.cmd = cmd
        self.env = env

    def run(self):
        self.set_status('running')
        self.send_output(self.cmd, 'cmd')
        code = compile(self.cmd, '<interaction>', 'exec')

        err = False
        try:
            exec code in self.env
        except Exception, ex:
            if isinstance(ex, greenlet.GreenletExit):
                raise
            else:
                traceback.print_exc()
                err = True
        finally:
            if not err:
                self.set_status('done')
                self.send_output('Done', 'status')
            else:
                self.set_status('error')

    def send_output(self, data, outtype):
        outtopic = topics.get('InteractionOutput')
        outtopic.send({'type': 'IOT_'+outtype.upper(), 'data': data+'\n'})

    def set_status(self, code):
        statustopic = topics.get('InteractionStatus')
        statustopic.send({'cmd': self.cmd, 'status': 'ISC_'+code.upper()})

class InteractionTask(sched.Task):
    def __init__(self):
        sched.Task.__init__(self, 'Interaction')
        self.cmdtask = None
        self.env = make_env()

    def run(self):
        while True:
            cmd = self.get_command()
            if self.cmdtask is not None:
                self.cmdtask.stop()
            if cmd['stop']:
                self.cmdtask = None
            else:
                self.cmdtask = CommandTask(cmd['cmd'], self.env)

    def get_command(self):
        cmdtopic = topics.get('InteractionCommand')
        while True:
            try:
                return cmdtopic.take()
            except dds.Error, err:
                sched.ddswait(cmdtopic)
                continue

class DDSInteractionStream(object):
    def __init__(self, outtype, realstream):
        self.outtype = outtype
        self.realstream = realstream

    def write(self, data):
        self.realstream.write(data)
        outtopic = topics.get('InteractionOutput')
        outtopic.send({'type': 'IOT_'+self.outtype.upper(), 'data': data})

def make_env():
    env = {}
    for modname in ['subjugator.nav', 'subjugator.sched', 'subjugator.topics', 'dds']:
        name = modname.split('.')[-1]
        env[name] = __import__(modname, fromlist=[modname])
    return env

interaction_task = InteractionTask()
sys.stdout = DDSInteractionStream('output', sys.stdout)
sys.stderr = DDSInteractionStream('error', sys.stderr)
