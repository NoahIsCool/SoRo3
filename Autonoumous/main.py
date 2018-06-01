import GPSNavigator
import time
import thread
from BallTrackerTest import BallTest



def getGPSCoor():
	gpsPair = [[5.7485935,5.75849025436], [3,3], [10,6]]
	for next in gpsPair:
		nav = GPSNavigator.GPSNavigator(next)
		numberOfFinds = 0
		while nav.distanceToTarget > 2 or numberOfFinds < 5:
			if nav.distanceToTarget <= 2:
				numberOfFinds += 1
			elif nav.distanceToTarget > 2:
				numberOfFinds = 0
			sleep(0.5)
		nav.nod()
		

thread.start_new_thread(getGPSCoor, ())

while True:
	command = raw_input("What value is to be changed? 'quit' to quit and 'cap' to save one frame from the camera ")
	if (command == "quit"):
		break;
	elif(command == "cap"):
		nav.bt.captureImage()

	elif(command == "h_lo"):
		nav.bt.h_lo = input("What is the is the value? ")
	elif(command == "h_hi"):
                nav.bt.h_hi = input("What is the is the value? ")
        elif(command == "s_lo"):
                nav.bt.s_lo = input("What is the is the value? ")
        elif(command == "s_hi"):
                nav.bt.s_hi = input("What is the is the value? ")
        elif(command == "v_lo"):
                nav.bt.v_lo = input("What is the is the value? ")
        elif(command == "v_hi"):
                nav.bt.v_hi = input("What is the is the value? ")
        elif(command == "e_itr"):
                nav.bt.e_itr = input("What is the is the value? ")
        elif(command == "e_box"):
                nav.bt.e_box = input("What is the is the value? ")
        elif(command == "d_itr"):
                nav.bt.d_itr = input("What is the is the value? ")
        elif(command == "d_box"):
                nav.bt.d_box = input("What is the is the value? ")
	
	elif(command == "values"):
		print "h_lo  = " + str(nav.bt.h_lo)
		print "h_hi  = " + str(nav.bt.h_hi)
		print "s_lo  = " + str(nav.bt.s_lo)
		print "s_hi  = " + str(nav.bt.s_hi)
		print "v_lo  = " + str(nav.bt.v_lo)
		print "v_hi  = " + str(nav.bt.v_hi)
		print "e_itr = " + str(nav.bt.e_itr)
		print "d_itr = " + str(nav.bt.d_itr)
		print "e_box = " + str(nav.bt.e_box)
		print "d_box = " + str(nav.bt.d_box)





