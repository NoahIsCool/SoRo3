
// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>

int main()
{
	double RADIUS_OF_EARTH = 6371 * (1000);

	// Location of rover. 0s are placeholders - need it in degrees
	double roverLat = 0;
	double roverLong = 0;

	// Get longitude, latitude, height, and number of satellites - need it in degrees
	// 0s are placeholders
	double longitude = 0;
	double latitude = 0;

	// convert longitude and latitude components to meters
	// Ask about difference in degree latitude
	double longMeters = longitude * RADIUS_OF_EARTH * cos(abs(latitude - roverLat)); // (latitude - roverLat)
	double latMeters = latitude * RADIUS_OF_EARTH;
	double longRovMeters = roverLong * RADIUS_OF_EARTH;
	double latRovMeters = roverLat * RADIUS_OF_EARTH ;

	// Probably need to incorporate magnitudes
	double latMeterTotal = (latMeters - latRovMeters);
	double longMeterTotal = (longMeters - longRovMeters);
	double tan;
	double distance;
	double bearingDiff;

	while ((longRovMeters != longMeters) && (latRovMeters != latMeters))
	{
		tan = (longMeterTotal / latMeterTotal);
		distance = sqrt(pow(longMeterTotal, 2) + pow(latMeterTotal, 2));
		bearingDiff = atan(tan);
		if (longMeterTotal < 0)
		{
			bearingDiff += 180;
		}
		// Make a turning method
		if (bearingDiff >= 90)
		{
			turnInPlace(bearingDiff);
		}
		else
		{
			turnInArc(bearingDiff);
		}
		//turn(bearing);
		// Make a drive method
		// drive(distance);

	}
	return 1;
}

int turnInPlace(double bearing)
{

	return 0;
}

int turnInArc(double bearing)
{

	return 0;
}
