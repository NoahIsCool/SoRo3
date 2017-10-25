
// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>

void main()
{
	// Location of rover. 0s are placeholders
	double roverLat = 0;
	double roverLong = 0;

	// Get longitude, latitude, height, and number of satellites
	// 0s are placeholders
	double longitude = 0;
	double latitude = 0;
	// convert longitude and latitude components to meters
	// 1s are place holders

	double longMeters = longitude * 1;
	double latMeters = latitude * 1;
	double longRovMeters = roverLong * 1;
	double latRovMeters = roverLat * 1;

	// Probably need to incorporate magnitudes
	double latMeterTotal = (latMeters - latRovMeters);
	double longMeterTotal = (longMeters - longRovMeters);
	double tan;
	double distance;
	double bearing;

	while ((longRovMeters != longMeters) && (latRovMeters != latMeters))
	{
		tan = (longMeterTotal / latMeterTotal);
		distance = sqrt(pow(longMeterTotal, 2) + pow(latMeterTotal, 2));
		bearing = atan(tan);
		if (longMeterTotal < 0)
		{
			bearing += 180;
		}
		// Make a turning method
		//turn(bearing);
		// Make a drive method
		// drive(distance);

	}
}
