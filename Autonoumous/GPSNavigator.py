import math
import argparse
# things for GPS communication
# make sure to run pip install sbp
from sbp.client.drivers.network_drivers import TCPDriver
from sbp.client import Handler, Framer
from sbp.navigation import SBP_MSG_POS_LLH, MsgPosLLH
from sbp.navigation import SBP_MSG_VEL_NED, MsgVelNED
from sbp.client.drivers.pyserial_driver import PySerialDriver

""" 
This code provides the rover with autonomous GPS navigation capabilities.
There will be two ways of inputting the destination coordinates:
    - User input from keyboard (Prone to user error, e.g. Being of by +/-.0004 longitude would result
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
        self.history = [10]
        self.args = self.get_args()
        self.rover_lat = 0.0
        self.rover_long = 0.0
        self.last_lat = 0.0
        self.last_long = 0.0
        self.longitude = 0.0
        self.latitude = 0.0
        self.bearing = 0.0
        self.distance = 0.0

        # start navigation method
        self.navigate()

    def navigate(self):
        # Select Driver
        if self.args.u is False:
            driver = TCPDriver('192.168.1.222', '55555')
        else:
            driver = PySerialDriver('/dev/ttyUSB0', baud=115200)  # 68,72,73,74,65535

        # Location of rover, from the top.
        # driver.read is the literal output from the tcp driver
        # framer turns bytes into SBP messages (swift binary protocol)
        # handler is an iterable to read the messages
        with Handler(Framer(driver.read, None, verbose=True)) as source:
            # reads all the messages in a loop as they are received
            for i in range(10):
                msg, metadata = source.filter(SBP_MSG_POS_LLH).next()
                self.history.append(msg)

            # Start Navigation Loop
            while True:
                # Get Next Driver Message
                msg, metadata = source.filter(SBP_MSG_POS_LLH).next()

                # Shouldn't We need an elevation?.. that's msg.height
                # int of accuracy is [h/v]_accuracy also there is n_sats
                self.last_lat = self.rover_lat
                self.last_long = self.rover_long
                self.rover_lat = math.radians(msg.lat)
                self.rover_long = math.radians(msg.lon)

                # Get longitude, latitude, (not yet...height, and number of satellites) - need it in degrees
                if self.args.bearing is None and self.args.distance is None:
                    self.longitude = math.radians(float(self.args.longitude))
                    self.latitude = math.radians(float(self.args.latitude))

                # If bearing and distance were supplied, get latitude and longitude in degrees
                else:
                    self.bearing = float(self.args.bearing)
                    self.distance = float(self.args.distance)
                    self.latitude, self.longitude = self.get_gps_coordinates()
                    self.latitude = math.radians(self.latitude)
                    self.longitude = math.radians(self.longitude)

                self.distance = self.get_distance()
                theta = self.get_target_bearing()
                delta_theta = self.get_rover_bearing() - theta

                # Get velocity in North, East, Down format
                msg2, metadata = source.filter(SBP_MSG_VEL_NED).next()

                # Velocity is in mm/s, so convert it to m/s
                vel__north = msg2.n * 1000
                vel__east = msg2.e * 1000

                print("North: ", vel__north, " East: ", vel__east)

                # Calculate rover_heading using the atan2 function
                rover_heading = math.atan2(vel__east, vel__north)

                # TODO: turn(bearing) # something like: if math.abs(bearing) > tolerance then turn else drive (distance)
                # TODO: drive(distance)

                # Need to fix this to output the proper value of theta
                print("Bearing: ", math.degrees(theta), ", Distance: ", self.distance,
                      ",Turn:", math.degrees(delta_theta), "My Bearing:", math.degrees(rover_heading))

                # If we're within 2 meters of the destination
                if self.distance <= 2:
                    print("Distance is within 2 meters of the destination.")
                    # TODO: put a break to say we are finished

    def get_gps_coordinates(self):
        # Convert the rover latitude and longitude to radians
        lat1 = math.radians(self.rover_lat)
        lon1 = math.radians(self.rover_long)
        bearing = math.radians(self.bearing)

        # Takes the lat1, lon2, distance, and bearing does the math needed
        lat2 = math.asin(math.sin(lat1) * math.cos(self.distance / self.RADIUS_OF_EARTH) +
                         math.cos(lat1) * math.sin(self.distance / self.RADIUS_OF_EARTH) * math.cos(bearing))
        lon2 = lon1 + math.atan2(math.sin(bearing) * math.sin(self.distance / self.RADIUS_OF_EARTH) * math.cos(lat1),
                                 math.cos(self.distance / self.RADIUS_OF_EARTH) - math.sin(lat1) * math.sin(lat2))

        # Change the latitude and longitude to degrees
        lat2 = math.degrees(lat2)
        lon2 = math.degrees(lon2)
        return lat2, lon2

    def get_distance(self):
        # Haversine Formula:
        # a = sin((lat2-lat1)/2) * sin((lat2-lat1)/2) + cos(lat1) * cos(lat2) * sin((lon2-lon1)/2) * sin((lon2-lon1)/2)
        # Calculate change in Latitude
        delta_lat = self.latitude - self.rover_lat
        # Calculate change in Longitude
        delta_lon = self.longitude - self.rover_long
        # Haversine formula for distance
        a = math.sin(delta_lat / 2) * math.sin(delta_lat / 2) + \
            math.cos(self.rover_lat) * math.cos(self.latitude) * math.sin(delta_lon / 2) * math.sin(delta_lon / 2)
        c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
        d = self.RADIUS_OF_EARTH * c
        return d

    def get_target_bearing(self):
        # Calculate change in Latitude
        # delta_lat = latitude - rover_lat
        # Calculate change in Longitude
        delta_lon = self.longitude - self.rover_long
        # Bearing, represented as nearest offset from North. (+/-180 degrees from North)
        theta = math.atan2(math.sin(delta_lon) * math.cos(self.latitude),
                           math.cos(self.rover_lat) * math.sin(self.latitude) -
                           math.sin(self.rover_lat) * math.cos(self.latitude) * math.cos(delta_lon))
        return theta

    def get_rover_bearing(self):
        # FIX THIS:
        # return math.atan2((rover_lat - last_long)/( roverLong - last_long))
        # Calculate change in Latitude
        # deltaLat = rover_lat - last_lat
        # Calculate change in Longitude
        delta_lon = self.rover_long - self.last_long
        # Bearing, represented as nearest offset from North. (+/-180 degrees from North)
        theta = math.atan2(math.sin(delta_lon) * math.cos(self.rover_lat),
                           math.cos(self.last_lat) * math.sin(self.rover_lat) -
                           math.sin(self.last_lat) * math.cos(self.rover_lat) * math.cos(delta_lon))
        return theta

    @staticmethod
    def get_args():
        """
        Get and parse arguments.
        """
        parser = argparse.ArgumentParser(
            description="Travel to a specified latitude and longitude")
        parser.add_argument(
            "--longitude",
            default=-122.41883,
            help="specify the longitude in decimal.")
        parser.add_argument(
            "--latitude",
            default=37.77432,
            help="specify the latitude in decimal.")
        parser.add_argument(
            "--bearing",
            help="specify the bearing to the destination in degrees.")
        parser.add_argument(
            "--distance",
            help="specify the distance to the destination in meters.")
        parser.add_argument(
            "-u",
            default=False,
            help="specify the usb port.")
        return parser.parse_args()
