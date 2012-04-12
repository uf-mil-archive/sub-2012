from subjugator import sched, sub

# Gathers data for hard and soft iron magnetometer calibration.
# Waits 30 seconds for the tether to be removed, then logs the magnetomter
# for 2 minutes as the sub is spun in circles at various pitch angles.

@sched.Task("log")
def log():
    sched.sleep(30)
    print "Logging"
    with sched.Timeout(2*60):
        with open('hard_soft.csv', 'w') as f:
            while True:
                sched.sleep(1.0/50)
                try:
                    f.write('%f,%f,%f' % sub.IMU.mag)
                except dds.Error as e:
                    pass
    print "Done"

sched.run()
