from subjugator import sched, topics
import random
import sys

class Test(sched.Task):
    def __init__(self, num):
        sched.Task.__init__(self, str(num))
        self.num = num

    def run(self):
        with sched.Timeout(random.random()*10 + 5) as timeout:
            while True:
                if random.random() < .1:
                    print self.num, "yield"
                    sched.yield_()
                elif random.random() < .1:
                    print self.num, "exit"
                    break
                else:
                    print self.num, "sleep"
                    sched.sleep(4*random.random())

        if timeout.activated:
            print self.num, "timed out"

tasks = map(Test, xrange(5))

@sched.Cycle("Tick", 1)
def cycle():
    print "Tick=========="

@sched.Task("log")
def logtask():
    topic = topics.get('WorkerLog')

    while True:
        sched.ddswait(topic)
        print topic.take()

@sched.Task("Join")
def jointask():
    for task in tasks:
        ok = sched.join(task)
        print "Join task joined", task.num

    print "Stopping the tick cycle"
    cycle.stop()
    logtask.stop()
    print "Join task ending"

sched.run()
