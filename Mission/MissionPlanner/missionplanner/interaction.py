from subjugator import topics
from subjugator import sched
from subjugator import worker
from subjugator import nav
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
        self.send_output(self.cmd, 'cmd')

        err = False
        try:
            try:
                expr = compile(self.cmd+'\n', '<interaction>', 'eval')
            except:
                expr = None
                code = compile(self.cmd, '<interaction>', 'exec')

            self.set_status('running')
            if expr is not None:
                self.send_output(repr(eval(expr, self.env)), 'status')
            else:
                eval(code, self.env)
                self.send_output('Done', 'status')
        except Exception, ex:
            traceback.print_exc()
            self.set_status('error')
            err = True
        finally:
            if not err:
                self.set_status('done')

    def send_output(self, data, outtype):
        outtopic = topics.get('InteractionOutput')
        outtopic.send({'type': 'IOT_'+outtype.upper(), 'data': data})

    def set_status(self, code):
        statustopic = topics.get('InteractionStatus')
        statustopic.send({'cmd': self.cmd, 'status': 'ISC_'+code.upper()})

class InteractionTask(sched.Task):
    def __init__(self):
        sched.Task.__init__(self, 'Interaction')
        self.cmdtask = None
        self.enabled = True
        self.env = make_env()

    def run(self):
        while True:
            cmd = self.get_command()
            if not self.enabled:
                continue
            if self.cmdtask is not None:
                self.cmdtask.stop()
            if cmd['stop']:
                self.cmdtask = None
                try:
                    nav.stop()
                except RuntimeError:
                    pass
            else:
                self.cmdtask = CommandTask(cmd['cmd'], self.env)

    def disable(self):
        self.enabled = False
        if self.cmdtask is not None:
            self.cmdtask.stop()
            self.cmdtask = None

    def enable(self):
        self.enabled = True

    def get_command(self):
        cmdtopic = topics.get('InteractionCommand')
        while True:
            try:
                return cmdtopic.take()
            except dds.Error, err:
                sched.ddswait(cmdtopic)
                continue

logfile = open('missionlog.txt', 'a')

class DDSInteractionStream(object):
    def __init__(self, outtype, realstream):
        self.outtype = outtype
        self.realstream = realstream
        self.buf = ''

    def write(self, data):
        self.realstream.write(data)
        logfile.write(data)
        logfile.flush()
        self.buf += data

        outtopic = topics.get('InteractionOutput')
        while True:
            (msg, newline, newbuf) = self.buf.partition('\n')
            if newline == '':
                break

            outtopic.send({'type': 'IOT_'+self.outtype.upper(), 'data': msg})
            self.buf = newbuf

def make_env():
    env = {}
    for modname in ['subjugator.nav', 'subjugator.sched', 'subjugator.topics', 'subjugator.sub', 'subjugator.vision', 'dds', 'math']:
        name = modname.split('.')[-1]
        env[name] = __import__(modname, fromlist=[modname])
    return env

interaction_task = InteractionTask()
sys.stdout = DDSInteractionStream('output', sys.stdout)
sys.stderr = DDSInteractionStream('error', sys.stderr)

worker.killmonitor.register_callbacks(lambda killedby: interaction_task.disable(), interaction_task.enable)
