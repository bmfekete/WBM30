/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDWind.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MD.h>

float MDWindAdjustment (float za,float disp,float z0,float fetch, float zw, float z0w) {
/* ratio of wind speed at reference height (above canopy) to wind speed at weather station after Brutsaert (1982)
 * za        - reference height [m]
 * disp      - height of zero-plane [m]
 * z0        - roughness parameter [m]
 * fetch     - fetch to weather station [m]
 * zw        - height of wind sensor at weather station [m]
 * z0w       - roughness parameter at weather station [m] */
	float hibl; /* height of internal boundary layer [m] */

 	hibl= 0.334 * pow ((double) fetch,0.875) * pow (z0w,0.125); /* Brutsaert equation 7-39 */

/* Brutsaert equations 7-41 and 4-3 */
   return (log (hibl / z0w) * log((za - disp) / z0) / (log (hibl / z0) * log (zw / z0w)));
}
