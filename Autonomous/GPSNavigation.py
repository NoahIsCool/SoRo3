import math
# things for GPS comunication
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

history = [10]

# TODO: move some variables to global variables to improve readability
RADIUS_OF_EARTH = 6371 * 1000


def main():
    args = get_args()
    rover_lat = 0.0
    rover_long = 0.0
    if args.u is False:
        driver = TCPDriver('192.168.1.222', '55555')
    else:
        driver = PySerialDriver('/dev/ttyUSB0', baud=115200)  # 68,72,73,74,65535
    # Location of rover, from the top.
    # driver.read is the literal output from the tcp driver
    # framer turns bytes into SBP messages (swift binary protocol)
    # handler is an iterable to read the messages
    with Handler(Framer(driver.read, None, verbose=True)) as source:
        # reads all the messages in a loop as they are received
        for i in range(1, 10):
            msg, metadata = source.filter(SBP_MSG_POS_LLH).next()
            history[i] = msg
        while True:
            msg, metadata = source.filter(SBP_MSG_POS_LLH).next()
            for i in range(1, 10):
                history[i-1] = history [i]
            history[9] = msg
            # Shouldnt We need an elevation?.. that's msg.height
            # int of acuracy is [h/v]_acuracy also there is n_sats
            last_lat = math.radians(history[0].lat)
            last_long = math.radians(history[0].lon)
            rover_lat = math.radians(msg.lat)
            rover_long = math.radians(msg.lon)

            # Get longitude, latitude, (not yet...height, and number of satellites) - need it in degrees
            if args.bearing is None and args.distance is None:
                longitude = math.radians(float(args.longitude))
                latitude = math.radians(float(args.latitude))

            # If bearing and distance were supplied, get latitude and longitude in degrees
            else:
                bearing = float(args.bearing)
                distance = float(args.distance)
                latitude, longitude = get_gps_coordinate(bearing, distance, rover_lat, rover_long)
                latitude = math.radians(latitude)
                longitude = math.radians(longitude)

            distance = get_distance(longitude, latitude, rover_lat, rover_long)
            theta = get_bearing(longitude, latitude, rover_lat, rover_long)
            delta_theta = get_my_bearing(rover_lat, rover_long, last_lat, last_long) - theta

            # Get velocity in North, East, Down format
            msg2, metadata = source.filter(SBP_MSG_VEL_NED).next()

            # Velocity is in mm/s, so convert it to m/s
            vel__north = msg2.n * 1000
            vel__east = msg2.e * 1000

            print("North: ", vel__north, " East: ", vel__east)

            # Calculate my_heading using the atan2 function
            my_heading = math.atan2(vel__east, vel__north)

            # TODO: turn(bearing) # something like: if math.abs(bearing) > tolerance then turn else drive (distance)
            # TODO: drive(distance)

            # Need to fix this to output the proper value of theta
            print("Bearing: ", math.degrees(theta), ", Distance: ", distance, ",Turn:", math.degrees(delta_theta),
                  "My Bearing:", math.degrees(my_heading))

            # If we're within 2 meters of the destination
            if distance <= 2:
                print("Distance is within 2 meters of the destination.")
                # TODO: put a break to say we are finished


def get_args():
    """
    Get and parse arguments.
    """
    import argparse
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


def get_distance(longitude, latitude, rover_lat, rover_long):
    # Radius of the Earth
    # TODO: Make RADIUS_OF_EARTH a class variable
    RADIUS_OF_EARTH = 6371000  # 6371 * 1000
    # Haversine Formula:
    # a = sin((lat2-lat1)/2) * sin((lat2-lat1)/2) + cos(lat1) * cos(lat2) * sin((lon2-lon1)/2) * sin((lon2-lon1)/2)
    # Calculate change in Latitude
    delta_lat = latitude - rover_lat
    # Calculate change in Longitude
    delta_lon = longitude - rover_long
    # Haversine formula for distance
    a = math.sin(delta_lat / 2) * math.sin(delta_lat / 2) + math.cos(rover_lat) * math.cos(latitude) * math.sin(
        delta_lon / 2) * math.sin(delta_lon / 2)
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
    d = RADIUS_OF_EARTH * c
    return d


def get_my_bearing(rover_lat, rover_long, last_lat, last_long):
    
    # return math.atan2((rover_lat - last_lat)/( roverLong - last_long))
    # Calculate change in Latitude
    deltaLat = rover_lat - last_lat
    # Calculate change in Longitude
    delta_lon = rover_long - last_long
    # Bearing, represented as nearest offset from North. (+/-180 degrees from North)
    theta = math.atan2(math.sin(delta_lon) * math.cos(rover_lat),
                       math.cos(last_lat) * math.sin(rover_lat) - math.sin(last_lat) * math.cos(rover_lat) * math.cos( delta_lon))
    return theta


def get_bearing(longitude, latitude, rover_lat, rover_long):
    # Calculate change in Latitude
    # delta_lat = latitude - rover_lat
    # Calculate change in Longitude
    delta_lon = longitude - rover_long
    # Bearing, represented as nearest offset from North. (+/-180 degrees from North)
    theta = math.atan2(math.sin(delta_lon) * math.cos(latitude),
                       math.cos(rover_lat) * math.sin(latitude) - math.sin(rover_lat) * math.cos(latitude) * math.cos(
                           delta_lon))
    return theta


def get_gps_coordinate(bearing, distance, rover_lat, rover_long):
    # Convert the rover latitude and longitude to radians
    lat1 = math.radians(rover_lat)
    lon1 = math.radians(rover_long)
    bearing = math.radians(bearing)

    # Takes the lat1, lon2, distance, and bearing does the math needed
    lat2 = math.asin(math.sin(lat1) * math.cos(distance / RADIUS_OF_EARTH) +
                     math.cos(lat1) * math.sin(distance / RADIUS_OF_EARTH) * math.cos(bearing))
    lon2 = lon1 + math.atan2(math.sin(bearing) * math.sin(distance / RADIUS_OF_EARTH) *
                             math.cos(lat1), math.cos(distance / RADIUS_OF_EARTH) - math.sin(lat1) * math.sin(lat2))

    # Change the latitude and longitude to degrees
    lat2 = math.degrees(lat2)
    lon2 = math.degrees(lon2)
    return lat2, lon2


def drive(distance):
    # TODO: Make a drive method
    return 0


def turn(bearing):
    # TODO: Make a turning method
    """
    if (bearingDiff >= 90):
        turn_in_place(bearingDiff)
    else:
        turn_in_arc(bearingDiff)
    """
    return 0


def turn_in_place(bearing):
    # TODO: finnish this
    return 0


def turn_in_arc(bearing):
    # TODO: finnish this
    return 0


if __name__ == "__main__":
    main()
