import greenlet
from collections import deque
import heapq
import time
import weakref

# Globals

class Sched:
    current_task = None
    sched_greenlet = greenlet.getcurrent()
    
    tasks = weakref.WeakSet()
    active_tasks = []
    active_tasks_nextpos = 0
    
    timers = [] # heap
    
    topics = set()

def current_task():
    return Sched.current_task
    
def _switch_sched():
    Sched.sched_greenlet.switch()

# Primitives

class Task:
    def __init__(self, name, func=None):
	self.func = func
	self.name = name
	self.greenlet = greenlet.greenlet(self._entry)
	self.waketasks = set()
	self.wait = None
	
	Sched.active_tasks.append(self)
	Sched.tasks.add(self)
	
    def __call__(self, func): # for decorator usage
	self.func = func
	return self

    def run(self):
	self.func()
	
    def suspend(self, wait):
	assert self.state == 'active'
	self.wait = wait
	
	self._remove_from_active()
	    
	if Sched.current_task == self:
	    _switch_sched()
	    
	self.wait = None
	    
    def resume(self):
	assert self.state == 'wait'
	Sched.active_tasks.append(self)
	
    def stop(self):
	if self.greenlet is None:
	    return 
	    
	self.greenlet.parent = greenlet.getcurrent()
	self.greenlet.throw()
    
    @property
    def state(self):
	if self.greenlet is None:
	    return 'stopped'
	elif self.wait is not None:
	    return 'wait'
	else:
	    return 'active'
	    
    def add_waketask(self, task):
	self.waketasks.add(task)
	
    def remove_waketask(self, task):
	self.waketasks.discard(task)
	    
    def _entry(self):
	try:
	    self.run()
	finally:
	    self.greenlet = None
	    for task in self.waketasks:
		task.resume()
	    
	    if self in Sched.active_tasks:
		self._remove_from_active()

    def _remove_from_active(self):
	pos = Sched.active_tasks.index(self)
	del Sched.active_tasks[pos]
	
	if Sched.active_tasks_nextpos > pos:
	    Sched.active_tasks_nextpos -= 1
	    
    def _switch(self, *args):
	Sched.current_task = self
	self.greenlet.switch(*args)
 
class Timer:
    def __init__(self, time, func=None):
	self.time = time
	self.func = func
	self.activated = False
	
	heapq.heappush(Sched.timers, self)
	
    def run(self):
	assert not self.activated
	self.activated = True
	self.func()
	
    def cancel(self):
	assert not self.activated
	Sched.timers.remove(self)
	heapq.heapify(Sched.timers)
	
    def get_remaining(self, curtime=None):
	if curtime is None:
	    curtime = time.time()
	return self.time - curtime
	
    def __cmp__(self, other):
	return cmp(self.time, other.time)

class TopicMonitor:
    def __init__(self, topic, func=None):
	self.topic = topic
	self.func = func
	self.activated = False
	Sched.topics.add(self)
    
    def run(self):
	assert not self.activated
	self.activated = True
	self.func()
	
    def cancel(self):
	Sched.topics.remove(self)

# Scheduler

def run():
    assert(Sched.sched_greenlet == greenlet.getcurrent())
    
    while len(Sched.active_tasks) > 0 or len(Sched.timers) > 0 or len(Sched.topics) > 0:
	_run_timers()
	_run_active_tasks()
	_run_block()
	
def _run_timers():
    ctr = 0
    while len(Sched.timers) > 0 and Sched.timers[0].get_remaining() <= .01:
	ctr += 1
	t = heapq.heappop(Sched.timers)
	t.run()

def _run_active_tasks():
    while Sched.active_tasks_nextpos < len(Sched.active_tasks):
	t = Sched.active_tasks[Sched.active_tasks_nextpos]
	Sched.active_tasks_nextpos += 1
	t._switch()
	
    Sched.active_tasks_nextpos = 0
	
def _run_block():
    # TODO Waitsets
    
    if len(Sched.active_tasks) > 0:
	sleeptime = 0
    elif len(Sched.timers) > 0:
	sleeptime = Sched.timers[0].get_remaining()
    else:
	sleeptime = 0
	
    if sleeptime > 0:
	time.sleep(sleeptime)

# High level

def sleep(secs):
    return sleep_until(time.time() + secs)

def sleep_until(secs):
    task = current_task()
    timer = Timer(secs, task.resume)
    
    try:
	task.suspend('sleep')
    finally:
	if not timer.activated:
	    timer.cancel()

def join(othertask):
    if othertask.state == 'stopped':
	return
    
    task = current_task()
    othertask.add_waketask(task)
    
    try:
	task.suspend('join')
    finally:
	othertask.remove_waketask(task)

def join_timeout(othertask, timeout):
    if othertask.state == 'stopped':
	return True

    task = current_task()
    timer = Timer(time.time() + timeout, task.resume)
    othertask.add_waketask(task)
    
    try:
	task.suspend('join_timeout')
	return not timer.activated
    finally:
	othertask.remove_waketask(task)
	
	if not timer.activated:
	    timer.cancel()

def yield_():
    _switch_sched()
    
class Signal:
    def __init__(self):
	self.wakelist = deque()
	
    def wait(self):
	task = current_task()
	self.wakelist.append(task)
	
	try:
	    task.suspend('Signal.wait')
	finally:
	    try:
		self.wakelist.remove(task)
	    except ValueError:
		pass
		
    def wait_timeout(self, timeout):
	task = current_task()
	self.wakelist.append(task)
	
	timer = Timer(time.time() + timeout, task.resume)
	
	try:
	    task.suspend('Signal.wait_timeout')
	    return not timer.activated
	finally:
	    try:
		self.wakelist.remove(task)
	    except ValueError:
		pass
		
	    if not timer.activated:
		timer.cancel()
		
    def notify(self):
	if len(self.wakelist) == 0:
	    return
	    
	task = self.wakelist.popleft()
	task.resume()
	
    def notify_all(self):
	for task in self.wakelist:
	    task.resume()
	del self.wakelist[:]

class Cycle:
    def __init__(self, name, period, func=None):
	self.func = func
	self.name = name
	self.period = period
	self.task = Task("cycle " + self.name, self._runloop)
	
    def __call__(self, func): # for decorator usage
	self.func = func
	return self
	
    def run(self):
	self.func()
	
    def _runloop(self):
	nexttime = time.time()
	
	while True:
	    nexttime += self.period
	    self.run()
	    
	    sleep_until(nexttime)
	    
    def stop(self):
	self.task.stop()
	    
