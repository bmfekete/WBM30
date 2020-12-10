/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDRainSurfRunoff.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Output
static int _MDOutRainInfiltrationID = MFUnset;
static int _MDOutRainSurfCore_RunoffID   = MFUnset;

int MDCore_RainSurfRunoffDef () {

	if (_MDOutRainSurfCore_RunoffID != MFUnset) return (_MDOutRainSurfCore_RunoffID);
	
	if (((_MDOutRainInfiltrationID = MDCore_RainInfiltrationDef()) == CMfailed) ||
        ((_MDOutRainSurfCore_RunoffID   = MFVarGetID (MDVarCore_RainSurfRunoff, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed))
		return (CMfailed);
	return (_MDOutRainSurfCore_RunoffID);
}
