import BallTracker
import time

bt = BallTracker.BallTracker()

while True:
    print str(bt.hasFound()) + ", " + str(bt.getAngle()) + ", " + str(bt.distance)
    time.sleep(1)    
