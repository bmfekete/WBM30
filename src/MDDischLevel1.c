/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDischLevel1.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInDischLevel2ID   = MFUnset;
static int _MDInDischReleasedID = MFUnset;

// Output
static int _MDOutDischLevel1ID = MFUnset;

static void _MDDischLevel1 (int itemID) {
	float discharge;

	if ((_MDInDischReleasedID != MFUnset) && (!MFVarTestMissingVal (_MDInDischReleasedID, itemID)))
		 discharge = MFVarGetFloat (_MDInDischReleasedID, itemID, 0.0);
	else discharge = MFVarGetFloat (_MDInDischLevel2ID,   itemID, 0.0);

	MFVarSetFloat (_MDOutDischLevel1ID, itemID, discharge);
}

int MDDischLevel1Def() {

   char *optStr;
   char *options [] = { MDNoneStr, (char *) NULL };

   
	if (_MDOutDischLevel1ID != MFUnset) return (_MDOutDischLevel1ID);

	MFDefEntering ("Discharge Level 1");
	if ((_MDInDischLevel2ID = MDDischLevel2Def ()) == CMfailed) return (CMfailed);

	if (((optStr = MFOptionGet (MDOptReservoirs)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
		if ((_MDInDischReleasedID = MDReservoirDef ()) == CMfailed) return (CMfailed);
	}
	if (((_MDOutDischLevel1ID = MFVarGetID ("__DischLevel1", "m3/s",  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDDischLevel1) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Discharge Level 1");
	return (_MDOutDischLevel1ID);
}
