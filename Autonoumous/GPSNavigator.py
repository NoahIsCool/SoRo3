import math
import argparse
# things for GPS communication
# make sure to run pip install sbp
import _thread
from sbp.client.drivers.network_drivers import TCPDriver
from sbp.client import Handler, Framer
from sbp.navigation import SBP_MSG_POS_LLH, MsgPosLLH
from sbp.navigation import SBP_MSG_VEL_NED, MsgVelNED
from sbp.client.drivers.pyserial_driver import PySerialDriver

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
        self.history = [10]
        self.args = self.get_args()
        self.rover_latitude = 0.0
        self.rover_longitude = 0.0
        self.last_rover_latitude = 0.0
        self.last_rover_longitude = 0.0
        self.target_longitude = 0.0
        self.target_latitude = 0.0
        self.target_bearing = 0.0
        self.distance = 0.0

        # start navigation method
        _thread.start_new_thread(self.navigate(), ())

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
                self.last_rover_latitude = self.rover_latitude
                self.last_rover_longitude = self.rover_longitude
                self.rover_latitude = math.radians(msg.lat)
                self.rover_longitude = math.radians(msg.lon)

                # Get target_longitude, target_latitude,
                # (not yet...height, and number of satellites) - need it in degrees
                if self.args.bearing is None and self.args.distance is None:
                    self.target_longitude = math.radians(float(self.args.longitude))
                    self.target_latitude = math.radians(float(self.args.latitude))

                # If target_bearing and distance were supplied, get target_latitude and target_longitude in degrees
                else:
                    self.target_bearing = float(self.args.bearing)
                    self.distance = float(self.args.distance)
                    self.target_latitude, self.target_longitude = self.get_gps_coordinates()
                    self.target_latitude = math.radians(self.target_latitude)
                    self.target_longitude = math.radians(self.target_longitude)

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

                # TODO: turn(target_bearing) # something like: if math.abs(target_bearing) > tolerance
                # TODO: then turn else drive (distance) drive(distance)

                # Need to fix this to output the proper value of theta
                print("Bearing: ", math.degrees(theta), ", Distance: ", self.distance,
                      ",Turn:", math.degrees(delta_theta), "My Bearing:", math.degrees(rover_heading))

                # If we're within 2 meters of the destination
                if self.distance <= 2:
                    print("Distance is within 2 meters of the destination.")
                    # TODO: put a break to say we are finished

    def get_gps_coordinates(self):
        # Convert the rover target_latitude and target_longitude to radians
        lat1 = math.radians(self.rover_latitude)
        lon1 = math.radians(self.rover_longitude)
        bearing = math.radians(self.target_bearing)

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
        # FIX THIS:
        # return math.atan2((rover_latitude - last_rover_longitude)/( roverLong - last_rover_longitude))
        # Calculate change in Latitude
        # deltaLat = rover_latitude - last_rover_latitude
        # Calculate change in Longitude
        delta_lon = self.rover_longitude - self.last_rover_longitude
        # Bearing, represented as nearest offset from North. (+/-180 degrees from North)
        theta = math.atan2(math.sin(delta_lon) * math.cos(self.rover_latitude),
                           math.cos(self.last_rover_latitude) * math.sin(self.rover_latitude) -
                           math.sin(self.last_rover_latitude) * math.cos(self.rover_latitude) * math.cos(delta_lon))
        return theta

    @staticmethod
    def get_args():
        """
        Get and parse arguments.
        """
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
            help="specify the usb port.")
        return parser.parse_args()