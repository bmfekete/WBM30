/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDSurfRunoff.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainSurfCore_RunoffID = MFUnset;
static int _MDInSmallResUptakeID = MFUnset;
static int _MDInWetlandUptakeID  = MFUnset;

// Output
static int _MDOutSurfCore_RunoffID    = MFUnset;

static void _MDSurfRunoff (int itemID) {	
// Input
	float surfRunoff; // Surface runoff [mm/dt]
	// TODO: The small reservoir uptake might exceed rain surface runoff
	surfRunoff = MFVarGetFloat (_MDInRainSurfCore_RunoffID,     itemID, 0.0)
	           - (_MDInSmallResUptakeID != MFUnset ? MFVarGetFloat (_MDInSmallResUptakeID, itemID, 0.0) : 0.0);
	MFVarSetFloat (_MDOutSurfCore_RunoffID,  itemID, surfRunoff);
}

int MDCore_SurfRunoffDef () {
	int ret;
	if (_MDOutSurfCore_RunoffID != MFUnset) return (_MDOutSurfCore_RunoffID);

	MFDefEntering ("Surface runoff");
	 
	if (((ret = MDReservoir_FarmPondReleaseDef()) != MFUnset) &&
        ((ret == CMfailed) ||
	     ((_MDInSmallResUptakeID = MFVarGetID (MDVarReservoir_FarmPontUptake, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed)))
	     return (CMfailed);
	
	if (((_MDInRainSurfCore_RunoffID  = MDCore_RainSurfRunoffDef()) == CMfailed) ||
        ((_MDOutSurfCore_RunoffID     = MFVarGetID (MDVarCore_SurfRunoff, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDSurfRunoff) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Surface runoff");
	return (_MDOutSurfCore_RunoffID);
}
