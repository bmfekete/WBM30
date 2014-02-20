/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDDischarge.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInDischLevel1ID   = MFUnset;
static int _MDInDischObservedID = MFUnset;

// Output
static int _MDOutDischargeID     = MFUnset;

static void _MDDischarge (int itemID) {
	float discharge; // Discharge [m3/s]

	discharge = MFVarGetFloat (_MDInDischLevel1ID,   itemID, 0.0);

	if (_MDInDischObservedID != MFUnset)
		 discharge = MFVarGetFloat (_MDInDischObservedID, itemID, discharge);

//	if (itemID == 1224 || itemID == 531) printf("**MDDischarge** itemID = %d, day = %d, discharge = %f\n", itemID, MFDateGetCurrentDay(), discharge);
	MFVarSetFloat (_MDOutDischargeID, itemID, discharge);
}

enum { MDinput, MDcalculate, MDcorrected };

int MDDischargeDef() {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptDischarge;
	const char *options [] = { MDInputStr, MDCalculateStr, "corrected", (char *) NULL };

	if (_MDOutDischargeID != MFUnset) return (_MDOutDischargeID);

	MFDefEntering ("Discharge");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput: _MDOutDischargeID = MFVarGetID (MDVarDischarge,     "m3/s",  MFInput,  MFState, MFBoundary); break;
		case MDcorrected:
			if ((_MDInDischObservedID   = MFVarGetID (MDVarDischObserved, "m3/s",  MFInput,  MFState, MFBoundary)) == CMfailed)
				return (CMfailed);
		case MDcalculate:
			if (((_MDOutDischargeID     = MFVarGetID (MDVarDischarge,     "m3/s",  MFRoute,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInDischLevel1ID    = MDDischLevel1Def ()) == CMfailed) ||
				(MFModelAddFunction (_MDDischarge) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("Discharge");
	return (_MDOutDischargeID);
}
