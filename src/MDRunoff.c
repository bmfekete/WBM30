/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDRunoff.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInSurfRunoffID = MFUnset;
static int _MDInBaseFlowID   = MFUnset;
static int _MDInRunoffCorrID = MFUnset;
// Output
static int _MDOutRunoffID    = MFUnset;

static void _MDRunoff (int itemID) {
// Input
	float baseFlow;
	float surfaceRO;
	float runoffCorr;

	baseFlow  = MFVarGetFloat (_MDInBaseFlowID,   itemID, 0.0);
	surfaceRO = MFVarGetFloat (_MDInSurfRunoffID, itemID, 0.0);
	runoffCorr = _MDInRunoffCorrID == MFUnset ? 1.0 : MFVarGetFloat (_MDInRunoffCorrID, itemID, 1.0);
	MFVarSetFloat (_MDOutRunoffID, itemID, (baseFlow + surfaceRO) * runoffCorr);
}
 
enum { MDinput, MDcalculate, MDcorrected };

int MDRunoffDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDVarRunoff;
	const char *options [] = { MDInputStr, MDCalculateStr, "corrected", (char *) NULL };

	if (_MDOutRunoffID != MFUnset) return (_MDOutRunoffID);

	MFDefEntering ("Runoff");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDinput: _MDOutRunoffID = MFVarGetID (MDVarRunoff,         "mm",     MFInput,  MFFlux, MFBoundary); break;
		case MDcorrected:
			if ((_MDInRunoffCorrID  = MFVarGetID (MDVarRunoffCorretion, MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed)
				return (CMfailed);
		case MDcalculate:		
			if (((_MDInBaseFlowID   = MDBaseFlowDef   ()) == CMfailed) ||
			    ((_MDInSurfRunoffID = MDSurfRunoffDef ()) == CMfailed) ||
				((_MDOutRunoffID    = MFVarGetID (MDVarRunoff,          "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
				(MFModelAddFunction (_MDRunoff) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("Runoff");
	return (_MDOutRunoffID);
}
