/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrUptakeRiver.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDOutIrrUptakeRiverID = MFUnset;

enum { MDnone, MDcalculate };

int MDIrrUptakeRiverDef() {
	int optID = MFUnset;
	const char *optStr, *optName = "IrrUptakeRiver";
	const char *options [] = { MDNoneStr, MDCalculateStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	if ((optID == MDnone) || (_MDOutIrrUptakeRiverID != MFUnset)) return (_MDOutIrrUptakeRiverID);

	if (optID == MDcalculate)
		return (MFVarGetID (MDVarIrrUptakeRiver,    "mm",  MFOutput, MFFlux,  MFBoundary));
	else MFOptionMessage (optName, optStr, options);
	return (CMfailed);
}
