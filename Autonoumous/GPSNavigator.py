import math
import argparse
import BallTracker
# things for GPS communication
# make sure to run pip install sbp
import thread
import serial
import socket
from sbp.client.drivers.network_drivers import TCPDriver
from sbp.client import Handler, Framer
from sbp.navigation import SBP_MSG_POS_LLH, MsgPosLLH
from sbp.navigation import SBP_MSG_VEL_NED, MsgVelNED
from sbp.client.drivers.pyserial_driver import PySerialDriver
import time
""" 
This code provides the rover with autonomous GPS navigation capabilities.
There will be two ways of inputting the destination coordinates:
    - User input from keyboard (Prone to user error, e.g. Being of by +/-.0004 target_longitude would result
        in an approximate positional error of +/-30 meters.
    - Read Lat/Lon decimal values from a file. (Will remove human error 
        (double/triple check values before running the code)
This code then takes the Lat/Lon values and uses the Haversine formula to calculate distance to destination and also 
    calculates Theta, which is the closest angle to the destination, offset from North. (e.g. +/-180 degrees from North)
"""

DEBUG = True
def debug(tag,message):
	if DEBUG is True:
		print tag + ": " + message

class GPSNavigator:
    #in meters...kinda important
    RADIUS_OF_EARTH = 6371000 

    def __init__(self,gpsPair):
        # initialize variables
        print "initializing"
	
	self.history = [0,0,0,0,0,0,0,0,0,0]            
        self.delta_theta = 0.000000000000000
        self.usbGPS = False
        self.rover_latitude = 0.0
        self.rover_longitude = 0.0
        self.last_rover_latitude = 0.0
        self.last_rover_longitude = 0.0
        self.target_longitude = gpsPair[0] #-97.438749000000000000
        self.target_latitude = gpsPair[1] #35.18971720000000000000
        self.target_bearing = None
        self.distance = None
        self.distanceToTarget = 10
        #self.leg_distance = 0.0
        self.hasControl = True
        #self.distance_traveled = 0.0
        self.speed = 0
        self.angle = 0 
        self.ip = "10.0.0.2"
        self.port = 1234
        self.bt = BallTracker.BallTracker()
        self.gimble_spin = chr(0)
        self.gimble_tilt = chr(0)
        print self.bt
        # start navigation method
        print("heading to " + str(self.target_longitude) + " " + str(self.target_latitude))
        thread.start_new_thread(self.navigate, ())
        thread.start_new_thread(self.communications, ())

    def navigate(self):
        # Select Driver
        if self.usbGPS is False:
            driver = TCPDriver('10.0.0.223', '55555')
        else:
            driver = PySerialDriver('/dev/ttyUSB0', baud=115200)  # 68,72,73,74,65535

        # Location of rover, from the top.
        # driver.read is the foundliteral output from the tcp driver
        # framer turns bytes into SBP messages (swift binary protocol)
        # handler is an iterable to read the messages
        with Handler(Framer(driver.read, None, verbose=True)) as source:
            # reads all the messages in a loop as they are received
            for i in range(10):
                msg, metadata = source.filter(SBP_MSG_POS_LLH).next()
                self.history[i] = msg           
            #if self.target_bearing is None and self.distance is None:
             #   self.target_longitude = math.radians(double(self.target_longitude))
             #   self.target_latitude = math.radians(double(self.target_latitude))

            #If target_bearing and distance were supplied, get target_latitude and target_longitude in degrees
            #else:
             #   self.target_latitude = math.radians(self.target_latitude)
             #   self.target_longitude = math.radians(self.target_longitude)
             #   self.distance = double(self.distance)
             #   self.target_bearing = self.get_bearing(self.target_longitude, self.target_latitude, self.rover_longitude, self.rover_latitude)
             #   self.target_latitude, self.target_longitude = self.get_gps_coordinates()

            # Start Navigation Loop
            print("heading to " + self.target_longitude + " " + self.target_latitude)
            while True:
                # Get Next Driver Message
                msg, metadata = source.filter(SBP_MSG_POS_LLH).next()
		for i in range(9):
                    self.history[i] = self.history[i+1]
		self.history[9] = msg
                # Shouldn't We need an elevation?.. that's msg.height
                # int of accuracy is [h/v]_accuracy also there is n_sats
                self.last_rover_latitude = math.radians(self.history[0].lat)
                self.last_rover_longitude = math.radians(self.history[0].lon)
                self.rover_latitude = math.radians(msg.lat)
                self.rover_longitude = math.radians(msg.lon)
                
                # Get target_longitude, target_latitude,217
                # (not yet...height, and number of satellites) - need it in degrees

                # self.distance = self.get_distance()
                # self.leg_distance = self.distance
                # self.distance_traveled = (self.leg_distance - self.distance) / self.leg_distance
                theta = self.get_bearing(math.radians(self.target_longitude), math.radians(self.target_latitude), self.rover_longitude, self.rover_latitude)
                self.delta_theta = self.get_bearing(self.rover_longitude, self.rover_latitude, self.last_rover_longitude, self.last_rover_latitude) - theta

                # Get velocity in North, East, Down format
                msg2, metadata = source.filter(SBP_MSG_VEL_NED).next()
		
                #changes the angle of the rover to center the tennis ball
                if self.distance < 100 :
                    found = self.bt.hasFound()
                    if found is True:
                        # print "ball found" 
                        # Give control to the ball tracker
                        self.bt.setControl(True)
                        self.hasControl = False
                    else:
                        #Give control to the GPS code
                        self.bt.setControl(False)
                        self.hasControl = True
                
                if self.hasControl is True:
                    # Drive the rover via GPS
                    self.angle = math.degrees(self.delta_theta)
                    self.speed = 65
                    # If we're within 2 meters of the destination
                    if self.get_distance() <= 2:
                        print("Distance is within 2 meters of the destination.")
                        self.distanceToTarget = self.get_distance()
                        # A tennis ball wasn't found on the way to the destination.
                        # Signal that the rover122.41883 has reached the target destination.
                        self.speed = 0
                    else:
                        self.speed = 65
                        
                else:
                    # print"BT has controll"
                    # Drive the rover via Ball Tracker
                    self.angle = self.bt.getAngle()
                    # If we're within 2 meters of the destination
                    if self.bt.distance <= 2:
                        print("Distance is within 2 meters of the destination.")
                        self.distanceToTarget = self.bt.distance
                        # A tennis ball wasn't found on the way to the destination.
                        # Signal that the rover has reached the target destination.
                        self.speed = 0
                    else:
                        self.speed = 65

    def get_gps_coordinates(self):
        # Convert the rover target_latitude and target_longitude to radians
        lat1 = math.radians(self.rover_latitude)
        lon1 = math.radians(self.rover_longitude)
        bearing = math.radians(self.target_bearing)

        # Takes the lat1, lon2, distance, and target_bearing does the math needed
	#AKA, determins the new lat and long in reference to the previous lat and long and the bearing
        lat2 = math.asin(math.sin(lat1) * math.cos(self.distance / self.RADIUS_OF_EARTH) + math.cos(lat1) * math.sin(self.distance / self.RADIUS_OF_EARTH) * math.cos(bearing))
        lon2 = lon1 + math.atan2(math.sin(bearing) * math.sin(self.distance / self.RADIUS_OF_EARTH) * math.cos(lat1), math.cos(self.distance / self.RADIUS_OF_EARTH) - math.sin(lat1) * math.sin(lat2))

        # Change the target_latitude and target_longitude to degrees
        lat2 = math.degrees(lat2)
        lon2 = math.degrees(lon2)
        return lat2, lon2

    def get_distance(self):
        # Haversine Formula:
        # a = sin((lat2-lat1)/2) * sin((lat2-lat1)/2) + cos(lat1) * cos(lat2) * sin((lon2-lon1)/2) * sin((lon2-lon1)/2)
        # Calculate change in Latitude
        delta_lat = math.radians(self.target_latitude) - self.rover_latitude
        # Calculate change in Longitude
        delta_lon = math.radians(self.target_longitude) - self.rover_longitude
        # Haversine formula for distance
	#https://en.wikipedia.org/wiki/Haversine_formula
	#similar to the normal distance formula but for spherical coordinates.
	#contrary to popular belief, the earth is round so we have to use spherical and not geometrical
        a = math.sin(delta_lat / 2) * math.sin(delta_lat / 2) + \
            math.cos(self.rover_latitude) * math.cos(self.target_latitude) * \
            math.sin(delta_lon / 2) * math.sin(delta_lon / 2)
        c = 2 * math.atan2(math.sqrt(abs(a)), math.sqrt(1 - abs(a)))
        d = self.RADIUS_OF_EARTH * c
        return d

    #calculates the change in latitude over the change in longitude
    #targets are the desired lat and long and last is the current lat and long
    def get_bearing(self,target_longitude,target_latitude,last_longitude,last_latitude):
	#someone found this online and it calcuates the bearing. Essentually black magic
	#debug("BEARING", "bearing targets: [" + target_longitude + "," + target_latitude + "]")
	#debug("BEARING", "bearing current: [" + last_longitude + "," + last_latitude + "]")
	delta_lon = target_longitude - last_longitude
	y = math.sin(delta_lon) * math.cos(target_latitude)
	x = math.cos(last_latitude) * math.sin(target_latitude) - math.sin(last_latitude) * math.cos(target_latitude) * math.cos(delta_lon)
	#debug("BEARING", "delta longitude: " + delta_lon)
	#debug("BEARING", "desired y: " + y)
	#debug("BEARING", "desired x: " + x)
        # Bearing, represented as nearest offset from North. (+/-180 degrees from North)
        theta = math.atan2(y,x)
        return theta

    def nod(self):
        self.gimble_tilt = 1
	sleep(1)
        self.gimble_tilt = -1
	sleep(1)
	self.gimble_tilt = 0

    def communications(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        #print "connecting..."
        #sock.bind((self.ip, self.port))
        #print "connected"
        hach = 0
        while True:
            out = ""
            # start, device index, gimble spin, gimble tilt 
            out = out + chr(129) + self.gimble_spin + self.gimble_tilt 
            if self.angle < -10:
                # Turn left
                out = out + chr(self.speed / 4) + chr(self.speed)
                hach = ((self.speed / 4)+ self.speed +90)
            else:
                if self.angle > 10:
                    # Turn right
                    out = out + chr(self.speed) + chr(self.speed/4)
                    hach = ((self.speed/4) + self.speed+90)
                else:
                    # Drive Forward
                    out = out + chr(self.speed) + chr(self.speed)
                    hach = ((2*self.speed)+90)
            hach = (hach +self.gimble_spin + self.gimble_tilt) / 5
            out = out + chr(90) + chr(0) + chr(hach)
            sock.sendto(out, (self.ip, self.port))
            print out
            print "Found Ball? " + str(self.bt.found) +str(self.distance) + ", " + str(self.delta_theta) +", " + str(self.speed) + ", target: " + str(self.target_longitude)+ ", " + str(self.target_latitude) + " my position: " + str(self.rover_latitude)+ ", " + str(self.rover_longitude)+ str(self.history[9]) 
            time.sleep(.1) 
