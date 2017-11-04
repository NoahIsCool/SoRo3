import math

def main(longitude, latitude, roverLat, roverLong):
    RADIUS_OF_EARTH = 6371 * (1000);

    #Location of rover. 0s are placeholders - need it in degrees
    roverLat = 0.0
    roverLong = 0.0

    #Get longitude, latitude, height, and number of satellites - need it in degrees
    #0s are placeholders
    longitude = 0.0
    latitude = 0.0

    # convert longitude and latitude components to meters
    # Ask about difference in degree latitude
    longMeters = longitude * RADIUS_OF_EARTH * math.cos(math.fabs(latitude - roverLat)) #(latitude - roverLat)
    latMeters = latitude * RADIUS_OF_EARTH
    longRovMeters = roverLong * RADIUS_OF_EARTH
    latRovMeters = roverLat * RADIUS_OF_EARTH 

    # Probably need to incorporate magnitudes
    latMeterTotal = (latMeters - latRovMeters)
    longMeterTotal = (longMeters - longRovMeters)
    tangent = 0.0
    distance = 0.0
    bearingDiff = 0.0
    while ((longRovMeters != longMeters) & (latRovMeters != latMeters)):
        tangent = (longMeterTotal / latMeterTotal)
    distance = math.sqrt(math.pow(longMeterTotal, 2) + math.pow(latMeterTotal, 2))
    bearingDiff = math.atan(tangent)
    if (longMeterTotal < 0):
        bearingDiff += 180
        # Make a turning method
        """if (bearingDiff >= 90):
            turnInPlace(bearingDiff)
    else:
        turnInArc(bearingDiff)"""
        #turn(bearing)
        # Make a drive method
        #drive(distance)
    print("Bearing: ", bearingDiff, ", Distance: ", distance)
    if (distance <= 2):
        print("Distance is within 2 meters of the destination.")
if(__name__=="__main__"):
    main(0, 0, 0, 0)

"""def turnInPlace(bearing):
    return 0;

def turnInArc(bearing):
    return 0;"""

