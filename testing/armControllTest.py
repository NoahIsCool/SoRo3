from inputs import get_gamepad
import socket
import time
import thread

# create an INET, STREAMing socket
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#now connect to the web server on port 80 - the normal http port
#s.connect(("127.255.255.255", 80))

controll = [  0,  0,  0, 0,  0]
stow    =   [100,100,142,96,100]
pos = [100,100,142,96,100]
#out   =   [  0,  0,  0, 0,  0]
out = ""
MAX_SPEED = .10 #degrees per mili second
MAX_INPUT = 32768.0


ARM_ADDRESS = ('10.194.16.12', 50007)


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.connect(ARM_ADDRESS)
def messenger():
	while True:
		out = str(int(pos[0]));

		for x in range(1,5):
			out = out + ',' + str(int(pos[x]))
		out = out + '\n'
		#sock.send(out)
		time.sleep(.1)
		print out

def posMonitor():
	while True:
		for x in range(0,5):
			pos[x] += controll[x]
			time.sleep(0.001)
def controller():
	while True:
		events = get_gamepad()
		for event in events:
			#          ###---BUTTON---##        ##############---DEAD ZONE---##########
			if event.code is 'ABS_Y' and not (-5000 < event.state and event.state < 5000):#shoulder L/R
				controll[0] = (float(event.state) / MAX_INPUT) * MAX_SPEED
			else:
				if event.code is 'ABS_Y' and (-5000 < event.state and event.state < 5000):
					controll[0] = 0
					
			if event.code is 'ABS_X' and not (-5000 < event.state < 5000):#shulder U/D              					    
				controll[1] = (float(event.state) / MAX_INPUT) * MAX_SPEED
			else:
				if event.code is 'ABS_X' and (-5000 < event.state and event.state < 5000):
					controll[1] = 0
					
			if event.code is 'ABS_RY' and not( -5000 < event.state < 5000):#elbow Y
				controll[2] = (float(event.state) / MAX_INPUT) * MAX_SPEED
			else:
				if event.code is 'ABS_RY' and (-5000 < event.state and event.state < 5000):
					controll[2] = 0
					
			if event.code is 'ABS_Z' and not( -5 < event.state < 5):#claw down
				controll[3] = (float(event.state) / 255) * MAX_SPEED
			else:
				if event.code is 'ABS_Z' and (-5000 < event.state and event.state < 5000):
					controll[3] = 0
					
			if event.code is 'ABS_RZ' and not( -5 < event.state < 5):#claw up
				controll[3] = -(float(event.state) / 255) * MAX_SPEED
			else:
				if event.code is 'ABS_RZ' and (-5000 < event.state and event.state < 5000):
					controll[3] = 0
			if event.code is 'BTN_TL' and event.state is 1:#claw grab
				pos[4] = 120
			else:
				if event.code is 'BTN_TL' and event.state is 0:#claw open
					pos[4] = 100
			if event.code is 'BTN_START':
				pos = stow
				

thread.start_new_thread( controller, () )
thread.start_new_thread( posMonitor, () )
thread.start_new_thread( messenger, () )



	
	
	

##	   if event.code is not 'SYN_REPORT':
##          if event.code is 'ABS_HAT0X':
##          if event.code is 'ABS_HAT0Y':
##          if event.code is 'BTN_TR':
##          if event.code is 'BTN_NORTH':
##          if event.code is 'BTN_SOUTH':
##          if event.code is 'BTN_EAST':
##          if event.code is 'BTN_WEST':
##          if event.code is 'BTN_START':
##          if event.code is 'BTN_SELECT':
##          if event.code is 'BTN_THUMBL':
##          if event.code is 'BTN_THUMBR':
#if event.code is 'ABS_RX':##if the arm had an elbow x

