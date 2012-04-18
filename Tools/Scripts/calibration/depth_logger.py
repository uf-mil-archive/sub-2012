from subjugator import sched, sub

# Records the depth sensor to a csv file at 50hz
# Used to calibrate the depth sensor by pushing the sub down to various known depths
# and finding the sensor reading on the resulting plot

@sched.Task("log")
def log():
    with open('depth.csv', 'w') as f:
        while True:
            sched.sleep(1.0/50)
            f.write('%f' % sub.DepthSensor.depth)

sched.run()
