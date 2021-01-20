/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDSolarRad.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

static float _MDSRadISC (int doy) {
	float isc;
	isc = 1.0 - .0167 * cos((double) 0.0172 * (double) (doy - 3));
	return (1367.0 / (isc * isc));
}

static float _MDSRadDEC (int doy) {
	double dec;

	dec = sin (6.224111 + 0.017202 * (double) doy);
	dec = 0.39785 * sin (4.868961 + .017203 * (double) doy + 0.033446 * sin (dec));
	return ((float) asin (dec));
}

static float _MDSRadH (float lat,int doy,float dec) {
	float arg, h;
	
   arg = -tan(dec) * tan(lat);
	if (arg > 1.0) h = 0.0;         /* sun stays below horizon */ 
	else if (arg <  -1.0) h = M_PI; /* sun stays above horizon */
	else h = acos(arg);
	return (h);
}

static int _MDOutCommon_SolarRadDayLengthID = MFUnset;

static void _MDCommon_SolarRadDayLength (int itemID) {
/* daylength fraction of day */
// Input
	int doy;   // day of the year
	float lat; // latitude in decimal radians
// Local
	float dec;
// Output
	float dayLength;

	doy = MFDateGetDayOfYear ();
	lat = MFModelGetLatitude (itemID) / 180.0 * M_PI;

	
   dec = _MDSRadDEC (doy);

   if (fabs ((double) lat) > M_PI_2) lat = (M_PI_2 - (double) 0.01) * (lat > 0.0 ? 1.0 : -1.0);

	dayLength = _MDSRadH (lat,doy,dec) / M_PI;
	MFVarSetFloat (_MDOutCommon_SolarRadDayLengthID,itemID,dayLength);
}

int MDCommon_SolarRadDayLengthDef () {
	if (_MDOutCommon_SolarRadDayLengthID != MFUnset) return (_MDOutCommon_SolarRadDayLengthID);

	MFDefEntering ("Day length");
	if (((_MDOutCommon_SolarRadDayLengthID   = MFVarGetID (MDVarCore_SolarRadDayLength, "1/d", MFOutput, MFState, false)) == CMfailed) ||
		(MFModelAddFunction(_MDCommon_SolarRadDayLength) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Day length");
	return (_MDOutCommon_SolarRadDayLengthID);
}

static int _MDOutCommon_SolarRadI0HDayID = MFUnset;

static void _MDSRadI0HDay (int itemID) {
/* daily potential solar radiation from Sellers (1965) */
// Input
	int   doy; // day of the year
	float lat; // latitude in decimal degrees
// Local
	float isc, dec, h;
// Output
	float i0hDay;

	doy = MFDateGetDayOfYear ();
	lat = MFModelGetLatitude (itemID) / 180.0 * M_PI;

	isc = _MDSRadISC (doy);
   dec = _MDSRadDEC (doy);

   if (fabs ((double) lat) > M_PI_2) lat = (M_PI_2 - (double) 0.01) * (lat > 0.0 ? 1.0 : -1.0);
	h = _MDSRadH (lat,doy,dec);

	i0hDay =	0.000001 * isc * (86400.0 / M_PI) *  (h * sin(lat) * sin(dec) + cos(lat) * cos(dec) * sin(h));
   MFVarSetFloat (_MDOutCommon_SolarRadI0HDayID,itemID,i0hDay);
}

int MDCommon_SolarRadI0HDayDef () {
	if (_MDOutCommon_SolarRadI0HDayID != MFUnset) return (_MDOutCommon_SolarRadI0HDayID);

	MFDefEntering ("I0H Day");
	if (((_MDOutCommon_SolarRadI0HDayID   = MFVarGetID (MDVarCore_SolarRadI0HDay, "MJ/m2", MFOutput, MFState, false)) == CMfailed) ||
        (MFModelAddFunction (_MDSRadI0HDay) == CMfailed)) return (CMfailed);
	MFDefLeaving ("I0H Day");
	return (_MDOutCommon_SolarRadI0HDayID);
}

static int _MDInputID, _MDCommon_GrossRadID;

static int _MDOutCommon_SolarRadID = MFUnset;

static void _MDSolarRadiationCloud (int itemID) {
// Input
	float cloud;
// Output
	float solarRad;
// Local

	solarRad = MFVarGetFloat (_MDCommon_GrossRadID, itemID, 0.0);
	cloud    = MFVarGetFloat (_MDInputID,    itemID, 0.0) ;
	if (fabs(cloud) > 100.0) printf ("cloud cover item %i  %f VarID %i \n",itemID, cloud, _MDInputID);
	cloud = cloud / 100.0;
		 
	solarRad = solarRad * (0.803 - (0.340 * cloud) - (0.458 * (float) pow ((double) cloud,(double) 2.0)));
    MFVarSetFloat (_MDOutCommon_SolarRadID,  itemID, solarRad);
}

static void _MDSolarRadiationSun (int itemID) {
// Input
	float sunShine;
// Output
	float solarRad;
// Local

	solarRad = MFVarGetFloat (_MDCommon_GrossRadID, itemID,  0.0);
	sunShine = MFVarGetFloat (_MDInputID,    itemID, 50.0) / 100.0;

	solarRad = solarRad * (0.251 + 0.509 * sunShine);
	MFVarSetFloat (_MDOutCommon_SolarRadID,  itemID, solarRad);
}

enum { MDinput, MDcloud, MDsun };

int MDCommon_SolarRadDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCore_SolarRadiation;
	const char *options [] = { MDInputStr, "cloud", "sun", (char *) NULL };

	if (_MDOutCommon_SolarRadID != MFUnset) return (_MDOutCommon_SolarRadID);

	MFDefEntering ("Solar Radiation");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDinput: _MDOutCommon_SolarRadID = MFVarGetID (MDVarCore_SolarRadiation, "MJ/m^2", MFInput, MFState, MFBoundary); break;
		case MDcloud:
		//	printf ("Option: Cloud!!!");
			if (((_MDCommon_GrossRadID     = MDCommon_GrossRadDef ())  == CMfailed) ||
                ((_MDInputID               = MDCommon_CloudCoverDef()) == CMfailed) ||
                ((_MDOutCommon_SolarRadID  = MFVarGetID (MDVarCore_SolarRadiation, "MJ/m^2", MFOutput,  MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDSolarRadiationCloud) == CMfailed)) return (CMfailed);
			break;
		case MDsun:
			if (((_MDCommon_GrossRadID     = MDCommon_GrossRadDef()) == CMfailed) ||
                ((_MDInputID               = MFVarGetID (MDVarCore_SunShine,       "%",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutCommon_SolarRadID  = MFVarGetID (MDVarCore_SolarRadiation, "MJ/m^2", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDSolarRadiationSun) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Solar Radiation");
	return (_MDOutCommon_SolarRadID);
}
