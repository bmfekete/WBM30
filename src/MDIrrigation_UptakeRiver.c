/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrUptakeRiver.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDOutIrrigation_UptakeRiverID = MFUnset;

enum { MDnone, MDcalculate };

int MDIrrigation_UptakeRiverDef() {
	int optID = MFUnset;
	const char *optStr, *optName = "IrrUptakeRiver";
	const char *options [] = { MDNoneStr, MDCalculateStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	if ((optID == MDnone) || (_MDOutIrrigation_UptakeRiverID != MFUnset)) return (_MDOutIrrigation_UptakeRiverID);

	if (optID == MDcalculate)
		return (MFVarGetID (MDVarIrrigation_UptakeRiver, "mm", MFOutput, MFFlux, MFBoundary));
	else MFOptionMessage (optName, optStr, options);
	return (CMfailed);
}
