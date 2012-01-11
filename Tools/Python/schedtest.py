from subjugator import sched
import random
import sys

class Test(sched.Task):
    def __init__(self, num):
	sched.Task.__init__(self, str(num))
	self.num = num
	
    def run(self):
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

tasks = [Test(i) for i in xrange(5)]

@sched.Cycle("Tick", 1)
def cycle():
    print "Tick=========="

@sched.Task("Join")
def jointask():
    for task in tasks:
	ok = sched.join_timeout(task, 5)
	if ok:
	    print "Join task joined", task.num
	else:
	    print "Join task stopping", task.num
	    task.stop()
    
    print "Stopping the cycle"
    cycle.stop()
    print "Join task ending"

sched.run()
print "Scheduler ended"
