import math
# import GPSComms as gps_c

""" This code provides the rover with autonomous GPS navigation capabilites.
    There will be two ways of inputting the destination coordinates:
        - User input from keyboard (Prone to user error, e.g. Being of by +/-.0004 longitude would result
            in an approximate positional error of +/-30 meters.
        - Read Lat/Lon decimal values from a file. (Will remove human error (double/triple check values before running the code)
    This code then takes the Lat/Lon values and uses the Haversine formula to calculate distance to destination and also calculates
        Theta, which is the closest angle to the destination, offset from North. (e.g. +/-180 degrees from North)"""

def main():
    # Radius of the Earth
    RADIUS_OF_EARTH = 6371 * (1000);

    #Location of rover. 0s are placeholders - need it in degrees
    # roverLat = math.radians(getLat())
    # roverLong = math.radians(getLon())

    #Get longitude, latitude, height, and number of satellites - need it in degrees
    #0s are placeholders
    longitude = math.radians(0)
    latitude = math.radians(0)

    # Haversine Formula: a = sin((lat2-lat1)/2) * sin((lat2-lat1)/2) + cos(lat1) * cos(lat2) * sin((lon2-lon1)/2) * sin((lon2-lon1)/2)
    # Calulate change in Latitude
    deltaLat = latitude - roverLat
    # Calculate change in Longitude
    deltaLon = longitude - roverLong
    # Haversine formula for distance
    a = math.sin(deltaLat/2) * math.sin(deltaLat/2) + math.cos(roverLat) * math.cos(latitude) * math.sin(deltaLon/2) * math.sin(deltaLon/2)
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a))
    d = RADIUS_OF_EARTH * c

    # Bearing, represented as nearest offset from North. (+/-180 degrees from North)
    theta = math.atan2(math.sin(deltaLon) * math.cos(latitude), math.cos(roverLat) * math.sin(latitude) - math.sin(roverLat) * math.cos(latitude) * math.cos(deltaLon))

    # Make a turning method
    """if (bearingDiff >= 90):
        turnInPlace(bearingDiff)
    else:
        turnInArc(bearingDiff)"""
    #turn(bearing)
        
    # Make a drive method
    #drive(distance)

    #Need to fix this to output the proper value of theta
    print("Bearing: ", 360 + math.degrees(theta), ", Distance: ", d)

    # If we're within 2 meters of the destination
    if (distance <= 2):
        print("Distance is within 2 meters of the destination.")

if(__name__=="__main__"):
    main()

def getLat():
    # return gps_c.getLat()
def getLon():
    # return gps_c.getLon()
def getBearing():
    # return gps_c.getBearing()
    
"""def turnInPlace(bearing):
    return 0;

def turnInArc(bearing):
    return 0;"""

