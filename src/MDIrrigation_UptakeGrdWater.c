/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrUptakeGrdWater.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDOutCommon_IrrUptakeGrdWaterID = MFUnset;

enum { MDnone, MDcalculate };

int MDIrrigation_UptakeGrdWaterDef() {
	int optID = MFUnset;
	const char *optStr, *optName = "IrrUptakeGrdWater";
	const char *options [] = { MDNoneStr, MDCalculateStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	if ((optID == MDnone) || (_MDOutCommon_IrrUptakeGrdWaterID != MFUnset)) return (_MDOutCommon_IrrUptakeGrdWaterID);

	if (optID == MDcalculate)
		return (MFVarGetID (MDVarIrrigation_UptakeGrdWater, "mm", MFOutput, MFFlux, MFBoundary));
	else MFOptionMessage (optName, optStr, options);
	return (CMfailed);
}
