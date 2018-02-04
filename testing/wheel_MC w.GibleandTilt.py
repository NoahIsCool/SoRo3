from inputs import get_gamepad ## in c can use SDL check out http://lazyfoo.net/tutorials/SDL/
import socket
import time
import thread
import serial
import math

# create an INET, STREAMing socket
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#now connect to the web server on port 80 - the normal http port
#s.connect(("127.255.255.255", 80))

pos = [0, 0, 0, 0, 0, 0, 0]
out = ""
MAX_SPEED = 90 #degrees per mili second
MAX_INPUT = 32768.0

x = 0
y = 0
rightWheels = 0
leftWheels = 0
gimbleDown = 0
gimbleUp = 0
gimble = 0
dpadRight = 0
tilt = 0
overdrive = 0


ser = serial.Serial('/dev/ttyUSB0',9600,timeout = 1) 

##Networking stuff
##ARM_ADDRESS = ('192.168.1.5', 21)
##sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
##sock.connect(ARM_ADDRESS)

def ioValue(x, y):
	global MAX_INPUT
	outside = 90 * ( y/abs(y+.0000000000001))
	return int(outside - ((x*outside*2)/90)), int(outside)

def messenger():

        #sends out information int he following form:
        #gimble, tilt, leftWheels, rightWheesl, overdrive, hash
        
	global leftWheels
	global rightWheels

	global gimbleDown
	global gimbleUp
	global gimble
        
	while True:

		hach = leftWheels + rightWheels + gimble + tilt + overdrive


		out = chr(2) + str(gimble) + ',' + str(tilt) + ',' + str(leftWheels) + ',' + str(rightWheels) + ',' + str(overdrive) + ',' + str(hach) ## a hash to check for errors

		gimble = (-gimbleDown + -gimbleUp) 
		 

		try:
		#sock.send(out + '\n')
			ser.write(out )
		#data = sock.recv(2048)
		#print out + ">>>>>>" + data
		#toPrint = 'Left Wheels: ' + str(leftWheels) + ', ' + 'Right Wheels: ' + str(rightWheels) + ', ' + 'GD: ' + str(gimbleDown) + ', ' + 'GU: ' + str(gimbleUp) + ', ' + 'Tot: ' + str(gimble)
		#toPrint += ', ' + 'Tilt: ' + str(tilt) + ', ' + 'Overdrive: ' + str(overdrive)
		###out + " {" + ser.readline() + "}"

		#print toPrint
			print out + " {" + ser.readline() + "}"
			
		except:
			print 'no message'
		
		#time.sleep(.2)# this breaks everything when using serial

def controller():
	global leftWheels
	global rightWheels

	global gimbleDown
	global gimbleUp
	global dpadRight
	global tilt
	global overdrive
	
	while True:
		events = get_gamepad()
		for event in events:
			#print event.code
			#          ###---BUTTON---##        ##############---DEAD ZONE---##########
			if event.code is 'ABS_Y' and not (-3000 < event.state and event.state < 3000):#shoulder L/R
				if event.state > 0:
				    leftWheels = -int((event.state / MAX_INPUT) * 84)
				else:
				    leftWheels = -int((event.state / MAX_INPUT) * MAX_SPEED)
			else:
				if event.code is 'ABS_Y' and (-3000 < event.state and event.state < 3000):
					leftWheels = 0
					#pos[0] = 2
			if event.code is 'ABS_RY' and not (-3000 < event.state and event.state < 3000):#shoulder L/R
				if event.state > 0:
				    rightWheels = -int((event.state / MAX_INPUT) * 84)
				else:
				    rightWheels = -int((event.state / MAX_INPUT) * MAX_SPEED)
			else:
				if event.code is 'ABS_RY' and (-3000 < event.state and event.state < 3000):
					rightWheels = 0
					#pos[0] = 2

			if event.code is 'ABS_RZ' and not( -5 < event.state < 5):#claw up
				#pos[2] = -event.state
				gimbleUp = int(-event.state * 5 / 255)
			else:
				if event.code is 'ABS_RZ' and (-5 < event.state and event.state < 5):
					gimbleUp = 0
			if event.code is 'ABS_Z' and not( -5 < event.state < 5):#claw down
				#pos[2] = event.state
				gimbleDown = int(event.state * 5 /255)
			else:
				if event.code is 'ABS_Z' and (-5 < event.state and event.state < 5):
					gimbleDown = 0
			if event.code is 'BTN_EAST' and event.state is 1:#claw grab
				overdrive = 1
				print 'hello'
			else:
				if event.code is 'BTN_EAST': 
					overdrive = 0
					print 'hello'
				
			#if event.code is 'ABS_HAT0X':
				#dpadRight = event.state
			if event.code is 'ABS_HAT0Y':
				tilt = -event.state
				#invert to make up positive



thread.start_new_thread( controller, () )
thread.start_new_thread( messenger, () )

	
	
##			if event.code is 'ABS_Y' and not (-5000 < event.state < 5000):#shoulder U/D              					    
##				controll[1] = (float(event.state) / MAX_INPUT) * MAX_SPEED
##			else:
##				if event.code is 'ABS_Y' and (-5000 < event.state and event.state < 5000):
##					controll[1] = 0
##					
##			if event.code is 'ABS_RY' and not( -5000 < event.state < 5000):#elbow Y
##				controll[2] = -(float(event.state) / MAX_INPUT) * MAX_SPEED
##			else:
##				if event.code is 'ABS_RY' and (-5000 < event.state and event.state < 5000):
##					controll[2] = 0
##					
##			if event.code is 'ABS_Z' and not( -5 < event.state < 5):#claw down
##				controll[3] = (float(event.state) / 255) * MAX_SPEED
##			else:
##				if event.code is 'ABS_Z' and (-5000 < event.state and event.state < 5000):
##					controll[3] = 0
##					
##			
##			if event.code is 'BTN_TL' and event.state is 1:#claw grab
##				controll[4] = 115
##			else:
##				if event.code is 'BTN_TL' and event.state is 0:#claw open
##					controll[4] = 80
##			if event.code is 'BTN_MODE':# stow
##				pos[0] = 100
##				pos[1] = 100 
##				pos[2] = 142
##				pos[3] = 96
##				pos[4] = 100
##			if event.code is 'BTN_SELECT': # balances the servos and, stops making noise
##				pos[0] = 100
##				pos[1] = 69 
##				pos[2] = 98
##				pos[3] = 96
##				pos[4] = 110     
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

