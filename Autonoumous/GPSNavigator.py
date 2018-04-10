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
import smbus
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


class GPSNavigator:
    RADIUS_OF_EARTH = 6371000

    def __init__(self):
        # initialize variables
        print "initializing"
        self.history = [0,0,0,0,0,0,0,0,0,0]
        self.args = self.get_args()
        self.rover_latitude = 0.0
        self.rover_longitude = 0.0
        self.last_rover_latitude = 0.0
        self.last_rover_longitude = 0.0
        self.target_longitude = -97.441337
        self.target_latitude = 35.210045
        self.target_bearing = None
        self.distance = None
        self.leg_distance = 0.0
        self.hasControl = True
        self.distance_traveled = 0.0
        self.speed = 0
        self.angle = 0 
        self.ip = "10.0.0.3"
        self.port = 1234
        # start navigation method


        #compas things
        # Get I2C bus
        self.bus = smbus.SMBus(1)

        self.xMax = -32768;
        self.xMin = 32768;
        self.yMax = -32768;
        self.yMin = 32768;
        self.zMax = -32768;
        self.zMin = 32768;
        self.magDeclination = 63.51666

        # MAG3110 address, 0x0E(14)
        # Select Control register, 0x10(16)
        #       0x01(01)    Normal mode operation, Active mode
        self.bus.write_byte_data(0x0E, 0x10, 0x01)

        time.sleep(0.5)

        #start threads
        thread.start_new_thread(self.navigate, ())
        thread.start_new_thread(self.communications, ())
        while True:
            time.sleep(10)
	    i = 1

    def navigate(self):
        # Select Driver
        if self.args.u is False:
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
                
            # Search for a tennis ball
            bt = BallTracker.BallTracker()
            print bt
            

            # Start Navigation Loop
            while True:
                # Get Next Driver Message
                msg, metadata = source.filter(SBP_MSG_POS_LLH).next()
                for i in range(1, 10):
                    self.history[i-1] = self.history [i]
                self.history[9] = msg
                # Shouldn't We need an elevation?.. that's msg.height
                # int of accuracy is [h/v]_accuracy also there is n_sats
                self.last_rover_latitude = self.history[0].lat
                self.last_rover_longitude = self.history[0].lon
                self.rover_latitude = math.radians(msg.lat)
                self.rover_longitude = math.radians(msg.lon)

                # Get target_longitude, target_latitude,217
                # (not yet...height, and number of satellites) - need it in degrees
                if self.target_bearing is None and self.distance is None:
                    self.target_longitude = math.radians(float(self.target_longitude))
                    self.target_latitude = math.radians(float(self.target_latitude))

                # If target_bearing and distance were supplied, get target_latitude and target_longitude in degrees
                else:                    
                    self.distance = float(self.distance)
                    self.target_bearing = self.get_target_bearing()
                    self.target_latitude, self.target_longitude = self.get_gps_coordinates()
                    self.target_latitude = math.radians(self.target_latitude)
                    self.target_longitude = math.radians(self.target_longitude)
                    
                
                self.distance = self.get_distance()
                self.leg_distance = self.distance
                self.distance_traveled = (self.leg_distance - self.distance) / self.leg_distance
                theta = self.get_target_bearing()
                delta_theta = self.get_rover_bearing() - theta

                # Get velocity in North, East, Down format
                msg2, metadata = source.filter(SBP_MSG_VEL_NED).next()

                # Velocity is in mm/s, so convert it to m/s
                vel__north = msg2.n * 1000
                vel__east = msg2.e * 1000

                #print("North: ", vel__north, " East: ", vel__east)

                # Calculate rover_heading using the atan2 function
                rover_heading = math.atan2(vel__east, vel__north)

                # TODO: turn(target_bearing) # something like: if math.abs(target_bearing) > tolerance
                # TODO: then turn else drive (distance) drive(distance)

                # Need to fix this to output the proper value of theta
                #print("Bearing: ", math.degrees(theta), ", Distance: ", self.distance,
                #     ",Turn:", math.degrees(delta_theta), "My Bearing:", math.degrees(rover_heading))
                #print "Ball found: " + str(bt.hasFound()) + ", ball angle: " + str(bt.getAngle()) + ", ball dist: " + str(bt.distance)
                if self.distance_traveled > 0.75:
                    found = bt.hasFound()
                    if found is True:
                        print "ball found" 
                        # Give control to the ball tracker
                        bt.setControl(True)
                        self.hasControl = False
                    else:
                        #Give control to the GPS code
                        bt.setControl(False)
                        self.hasControl = True
                
                if self.hasControl is True or True:
                    # Drive the rover via GPS
                    self.angle = math.degrees(delta_theta)
                    #self.speed = 25
                    # If we're within 2 meters of the destination
                    if self.get_distance() <= 2:
                        print("Distance is within 2 meters of the destination.")
                        # A tennis ball wasn't found on the way to the destination.
                        # Signal that the rover122.41883 has reached the target destination.
                        self.speed = 0
                    else:
                        print("distance is " + str(self.get_distance()) + " angle is " + str(math.degrees(self.angle)))
                        self.speed = 25
                        
                if bt.hasFound() is True and False:
                    print"BT has controll"
                    # Drive the rover via Ball Tracker
                    self.angle = bt.getAngle()
                    # If we're within 2 meters of the destination
                    if bt.distance <= 2:
                        print("Distance is within 2 meters of the destination.")
                        # A tennis ball wasn't found on the way to the destination.
                        # Signal that the rover has reached the target destination.
                        self.speed = 0
                    else:
                        self.speed = 25
    
    def get_gps_coordinates(self):
        # Convert the rover target_latitude and target_longitude to radians
        lat1 = math.radians(self.rover_latitude)
        lon1 = math.radians(self.rover_longitude)
        bearing = math.radians(float(self.target_bearing))

        # Takes the lat1, lon2, distance, and target_bearing does the math needed
        lat2 = math.asin(math.sin(lat1) * math.cos(self.distance / self.RADIUS_OF_EARTH) +
                         math.cos(lat1) * math.sin(self.distance / self.RADIUS_OF_EARTH) * math.cos(bearing))
        lon2 = lon1 + math.atan2(math.sin(bearing) * math.sin(self.distance / self.RADIUS_OF_EARTH) * math.cos(lat1),
                                 math.cos(self.distance / self.RADIUS_OF_EARTH) - math.sin(lat1) * math.sin(lat2))

        # Change the target_latitude and target_longitude to degrees
        lat2 = math.degrees(lat2)
        lon2 = math.degrees(lon2)
        return lat2, lon2

    def get_distance(self):
        # Haversine Formula:
        # a = sin((lat2-lat1)/2) * sin((lat2-lat1)/2) + cos(lat1) * cos(lat2) * sin((lon2-lon1)/2) * sin((lon2-lon1)/2)
        # Calculate change in Latitude
        delta_lat = self.target_latitude - self.rover_latitude
        # Calculate change in Longitude
        delta_lon = self.target_longitude - self.rover_longitude
        # Haversine formula for distance
        a = math.sin(delta_lat / 2) * math.sin(delta_lat / 2) + \
            math.cos(self.rover_latitude) * math.cos(self.target_latitude) * math.sin(delta_lon / 2) * \
            math.sin(delta_lon / 2)
        c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
        d = self.RADIUS_OF_EARTH * c
        return d

    def get_target_bearing(self):
        # Calculate change in Latitude
        # delta_lat = target_latitude - rover_latitude
        # Calculate change in Longitude
        delta_lon = self.target_longitude - self.rover_longitude
        # Bearing, represented as nearest offset from North. (+/-180 degrees from North)
        theta = math.atan2(math.sin(delta_lon) * math.cos(self.target_latitude),
                           math.cos(self.rover_latitude) * math.sin(self.target_latitude) -
                           math.sin(self.rover_latitude) * math.cos(self.target_latitude) * math.cos(delta_lon))
        return theta

    def get_rover_bearing(self):
        time.sleep(0.05)
        # MAG3110 address, 0x0E(14)
        # Read data back from 0x01(1), 6 bytes
        # X-Axis MSB, X-Axis LSB, Y-Axis MSB, Y-Axis LSB, Z-Axis MSB, Z-Axis LSB
        self.data = self.bus.read_i2c_block_data(0x0E, 0x01, 6)

        # Convert the data
        self.xMag = self.data[0] * 256 + self.data[1]
        if self.xMag > 32767 :
            self.xMag -= 65536

        self.yMag = self.data[2] *256 + self.data[3]
        if self.yMag > 32767 :
            self.yMag -= 65536
    
        self.zMag = self.data[4] * 256 + self.data[5]
        if self.zMag > 32767 :
            self.zMag -= 65536
                            
        if self.xMag > self.xMax:
            self.xMax = self.xMag
        if self.xMag < self.xMin:
           self.xMin = self.xMag
        if self.yMag > self.yMax:
            self.yMax = self.yMag
        if self.yMag < self.yMin:
            self.yMin = self.yMag

        self.xRel = self.xMag - (self.xMax + self.xMin)/2
        self.yRel = self.yMag - (self.yMax + self.yMin)/2

        self.direction = math.atan2(self.xRel, self.yRel) * (180 / math.pi)
        self.direction = self.direction - self.magDeclination
        if self.direction < 0:
            self.direction = self.direction + 360
        return self.direction

        # FIX THIS:
        # return math.atan2((rover_latitude - last_rover_longitude)/( roverLong - last_rover_longitude))
        # Calculate change in Latitude
        # deltaLat = rover_latitude - last_rover_latitude
        # Calculate change in Longitude
        # delta_lon = self.rover_longitude - self.last_rover_longitude
        # Bearing, represented as nearest offset from North. (+/-180 degrees from North)
        # theta = math.atan2(math.sin(delta_lon) * math.cos(self.rover_latitude),
                          #  math.cos(self.last_rover_latitude) * math.sin(self.rover_latitude) -
                          #  math.sin(self.last_rover_latitude) * math.cos(self.rover_latitude) * math.cos# (delta_lon))
       # return theta

    def communications(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        print "connecting..."
        #sock.bind((self.ip, self.port))
        #print "connected"
        hach = 0
        while True:
            out = ""
            # start, device index, gimble spin, gimble tilt 
            out = out + chr(129) + chr(0) + chr(0) 
            if self.angle < -2:
                # Turn left
                out = out + chr(0) + chr(self.speed)
                hach = (self.speed+90)/5 
                
            else:
                if self.angle > 2:
                    # Turn right
                    out = out + chr(self.speed) + chr(0)
                    hach = (self.speed+90)/5 
                else:
                    # Drive Forward
                    out = out + chr(self.speed) + chr(self.speed)
                    hach = ((2*self.speed)+90)/5 
            out = out + chr(90) + chr(0) + chr(hach)
            sock.sendto(out, (self.ip, self.port))
            
            print str(self.angle) +", " + str(self.speed)
            time.sleep(.1) 

    @staticmethod
    def get_args():
        """
        Get and parse arguments.
        """
        #TODO: add one for hsv Values
        parser = argparse.ArgumentParser(
            description="Travel to a specified target_latitude and target_longitude")
        parser.add_argument(
            "--target_longitude",
            default=-122.41883,
            help="specify the target_longitude in decimal.")
        parser.add_argument(
            "--target_latitude",
            default=37.77432,
            help="specify the target_latitude in decimal.")
        parser.add_argument(
            "--target_bearing",
            help="specify the target_bearing to the destination in degrees.")
        parser.add_argument(
            "--distance",
            help="specify the distance to the destination in meters.")
        parser.add_argument(
            "-u",
            default=False,
            help="specify the to use usb port.")
        parser.add_argument(
            "-ip",
            default="192.168.1.13",
            help="specify the ip adress.")
        parser.add_argument(
            "-port",
            default=1234,
            help="specify the ip adress port.")
        return parser.parse_args()
