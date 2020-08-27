/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrUptakeGrdWater.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDOutIrrUptakeGrdWaterID = MFUnset;

enum { MDnone, MDcalculate };

int MDIrrUptakeGrdWaterDef() {
	int optID = MFUnset;
	const char *optStr, *optName = "IrrUptakeGrdWater";
	const char *options [] = { MDNoneStr, MDCalculateStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	if ((optID == MDnone) || (_MDOutIrrUptakeGrdWaterID != MFUnset)) return (_MDOutIrrUptakeGrdWaterID);

	if (optID == MDcalculate)
		return (MFVarGetID (MDVarIrrUptakeGrdWater,    "mm",  MFOutput, MFFlux,  MFBoundary));
	else MFOptionMessage (optName, optStr, options);
	return (CMfailed);
}
