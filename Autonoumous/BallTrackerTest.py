import BallTracker
import time

class BallTest:
	def __init__(self):
		self.bt = BallTracker.BallTracker()
		time.sleep(1)
		#self.bt.captureImage()

		#while True:
			#print str(self.bt.hasFound()) + ", " + str(self.bt.getAngle()) + ", " + str(self.bt.distance)
		#	time.sleep(1)    
