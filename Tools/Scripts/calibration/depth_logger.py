from subjugator import sched, topics
import dds

# Records the depth sensor to a csv file at 50hz
# Used to calibrate the depth sensor by pushing the sub down to various known depths
# and finding the sensor reading on the resulting plot

@sched.Task("log")
def log():
    depthtopic = topics.get('Depth')

    with open('depth.csv', 'w') as f:
        while True:
            sched.sleep(1.0/50)

            try:
                depth = depthtopic.read()
                print >>f, '%f' % depth['depth']
            except dds.Error as e:
                pass

sched.run()
