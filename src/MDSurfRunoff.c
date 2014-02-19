/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDSurfRunoff.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainSurfRunoffID = MFUnset;
static int _MDInSmallResUptakeID = MFUnset;
static int _MDInWetlandUptakeID = MFUnset;
// Output
static int _MDOutSurfRunoffID    = MFUnset;

static void _MDSurfRunoff (int itemID) {	
// Input
	float surfRunoff; // Surface runoff [mm/dt]
	
	surfRunoff = MFVarGetFloat (_MDInRainSurfRunoffID,     itemID, 0.0)
	           - (_MDInSmallResUptakeID != MFUnset ? MFVarGetFloat (_MDInSmallResUptakeID, itemID, 0.0) : 0.0);
	MFVarSetFloat (_MDOutSurfRunoffID,  itemID, surfRunoff);
}

int MDSurfRunoffDef () {
	int ret;
	if (_MDOutSurfRunoffID != MFUnset) return (_MDOutSurfRunoffID);

	MFDefEntering ("Surface runoff");
	 
	if (((ret = MDWetlandRunoffDef ()) != MFUnset) &&
	    ((ret == CMfailed) ||
	     ((_MDInWetlandUptakeID = MFVarGetID (MDVarWetlandSurfROUptake, "mm",   MFInput,  MFFlux, MFBoundary)) == CMfailed)))
	     return (CMfailed);
	
	
	if (((ret = MDSmallReservoirReleaseDef ()) != MFUnset) &&
	    ((ret == CMfailed) ||
	     ((_MDInSmallResUptakeID = MFVarGetID (MDVarSmallResUptake, "mm",   MFInput,  MFFlux, MFBoundary)) == CMfailed)))
	     return (CMfailed);
	
	if (((_MDInRainSurfRunoffID  = MDRainSurfRunoffDef ()) == CMfailed) ||
	    ((_MDOutSurfRunoffID     = MFVarGetID (MDVarSurfRunoff,     "mm",   MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
       (MFModelAddFunction (_MDSurfRunoff) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Surface runoff");
	return (_MDOutSurfRunoffID);
}
