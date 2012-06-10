import greenlet
from collections import deque
import heapq
import time
import weakref

import dds

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
    """
    Represents a cooperative task. Tasks can either be passed
    a function to run, or have their run method overriden in a base
    class.
    """
    def __init__(self, name, func=None):
	"""Constructs a task given a name, and optionally a function to run"""
        self.func = func
        self.name = name
        self.greenlet = greenlet.greenlet(self._entry, Sched.sched_greenlet)
        self.stopcallbacks = set()
        self.wait = None
        self.throwex = None

        Sched.active_tasks.append(self)
        Sched.tasks.add(self)

    def __call__(self, func): # for decorator usage
	"""Sets the function of a Task. Allows the Task constructor to be used as a decorator"""
        self.func = func
        return self

    def run(self):
	"""Override this in a subclass with the code to run in the Task"""
        self.func()

    def stop(self):
	"""Stops the task. Once stopped, the task cannot be restarted"""
        if self.greenlet is None:
            return

        self.greenlet.parent = greenlet.getcurrent()
        self.greenlet.throw()

    def throw(self, ex):
	"""Throws an exception in the task. The exception will appear to originate
	from whatever function the task was previously blocked on. The exception isn't
	thrown until the task is scheduled for execution.
	"""
        self.throwex = ex
        if self.state == 'wait':
            self._resume()

    @property
    def state(self):
	"""
	Indicates whether a task is stopped, waiting, or active.
	Note that all running tasks are active, but there is only one
	current task, which is given by current_task()
	"""
        if self.greenlet is None:
            return 'stopped'
        elif self.wait is not None:
            return 'wait'
        else:
            return 'active'

    def add_stopcallback(self, stopcallback):
	"""Adds a callback to be invoked when this task is stopped for any reason."""
        self.stopcallbacks.add(stopcallback)

    def remove_stopcallback(self, stopcallback):
	"""Removes a callback added with add_stopcallback"""
        self.stopcallbacks.discard(stopcallback)

    def _entry(self):
        try:
            self.run()
        finally:
            self.greenlet = None
            map(apply, self.stopcallbacks)

            if self in Sched.active_tasks:
                self._remove_from_active()

    def _suspend(self, wait):
        assert self.state == 'active'
        self.wait = wait

        self._remove_from_active()

        if Sched.current_task == self:
            _switch_sched()

        self.wait = None

    def _resume(self):
        assert self.state == 'wait'
        Sched.active_tasks.append(self)

    def _remove_from_active(self):
        pos = Sched.active_tasks.index(self)
        del Sched.active_tasks[pos]

        if Sched.active_tasks_nextpos > pos:
            Sched.active_tasks_nextpos -= 1

    def _switch(self):
        Sched.current_task = self
        if self.throwex is None:
            self.greenlet.switch()
        else:
	    ex = self.throwex
	    self.throwex = None
            self.greenlet.throw(ex)

class Timer:
    """A Timer causes the scheduler to invoke a callback at some time in the future.
    The function isn't invoked in a specific task, and thus cannot block. In most uses,
    the callback simply awakens one or more Tasks.
    """
    def __init__(self, time, func):
	"""Creates a timer which will be invoked when time.time() reaches time."""
        assert(callable(func))

        self.time = time
        self.func = func
        self.activated = False

        heapq.heappush(Sched.timers, self)

    def cancel(self):
	"""Causes the timer to not invoke its callback if it has not yet been activated"""
        if self.activated:
            return
        Sched.timers.remove(self)
        heapq.heapify(Sched.timers)

    def get_remaining(self, curtime=None):
	"""Returns the amount of time in seconds until the timer will be activated"""
        if curtime is None:
            curtime = time.time()
        return self.time - curtime

    def __cmp__(self, other):
        return cmp(self.time, other.time)

    def _run(self):
        assert not self.activated
        self.activated = True
        self.func()

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
        Sched.topics.discard(self)

# Scheduler

def run():
    assert(Sched.sched_greenlet == greenlet.getcurrent())

    while len(Sched.active_tasks) > 0 or len(Sched.timers) > 0 or len(Sched.topics) > 0:
        _run_timers()
        _run_active_tasks()
        _run_block()

    Sched.topics.clear()

    import topics
    del topics.part
    del topics.lib

def _run_timers():
    while len(Sched.timers) > 0 and Sched.timers[0].get_remaining() <= .01:
        t = heapq.heappop(Sched.timers)
        t._run()

def _run_active_tasks():
    while Sched.active_tasks_nextpos < len(Sched.active_tasks):
        t = Sched.active_tasks[Sched.active_tasks_nextpos]
        Sched.active_tasks_nextpos += 1
        t._switch()

    Sched.active_tasks_nextpos = 0

def _run_block():
    waitset = dds.WaitSet()
    for topicmon in Sched.topics:
        waitset.attach(topicmon.topic, topicmon)

    if len(Sched.active_tasks) > 0:
        sleeptime = 0
    elif len(Sched.timers) > 0:
        sleeptime = max(Sched.timers[0].get_remaining(), 0)
    else:
        sleeptime = -1

    result = waitset.wait(sleeptime)
    for topicmon in result:
        topicmon.run()
        Sched.topics.remove(topicmon)

# High level

def sleep(secs):
    return sleep_until(time.time() + secs)

def sleep_until(secs):
    task = current_task()
    timer = Timer(secs, task._resume)

    try:
        task._suspend('sleep')
    finally:
        timer.cancel()

def join(othertask):
    if othertask.state == 'stopped':
        return

    task = current_task()
    othertask.add_stopcallback(task._resume)

    try:
        task._suspend('join')
    finally:
        othertask.remove_stopcallback(task._resume)

def join_timeout(othertask, timeout):
    if othertask.state == 'stopped':
        return True

    task = current_task()
    timer = Timer(time.time() + timeout, task._resume)
    othertask.add_stopcallback(task._resume)

    try:
        task._suspend('join_timeout')
        return not timer.activated
    finally:
        othertask.remove_stopcallback(task._resume)
        timer.cancel()

def ddswait(topic):
    task = current_task()
    topicmon = TopicMonitor(topic, task._resume)

    try:
        task._suspend('ddswait')
    finally:
        topicmon.cancel()

def ddswait_timeout(topic, timeout):
    task = current_task()
    topicmon = TopicMonitor(topic, task._resume)
    timer = Timer(time.time() + timeout, task._resume)

    try:
        task._suspend('ddswait_timeout')
        return not timer.activated
    finally:
        topicmon.cancel()
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
            task._suspend('Signal.wait')
        finally:
            try:
                self.wakelist.remove(task)
            except ValueError:
                pass

    def wait_timeout(self, timeout):
        task = current_task()
        self.wakelist.append(task)

        timer = Timer(time.time() + timeout, task._resume)

        try:
            task._suspend('Signal.wait_timeout')
            return not timer.activated
        finally:
            try:
                self.wakelist.remove(task)
            except ValueError:
                pass

            timer.cancel()

    def notify(self):
        if len(self.wakelist) == 0:
            return

        task = self.wakelist.popleft()
        task._resume()

    def notify_all(self):
        for task in self.wakelist:
            task._resume()
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

class TimeoutException(Exception):
    def __init__(self, timeoutobj):
        self.timeoutobj = timeoutobj

class Timeout:
    def __init__(self, timeout):
        self.timeout = timeout
        self.task = current_task()
        self.activated = False

    def __enter__(self):
        self.timer = Timer(time.time()+self.timeout, self._timeout)
        return self

    def _timeout(self):
        self.task.throw(TimeoutException(self))
        self.activated = True

    def __exit__(self, type, value, traceback):
	if not self.timer.activated:
	    self.timer.cancel()
        return type == TimeoutException and value.timeoutobj == self
