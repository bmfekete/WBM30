/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDichAccumulate.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInRunoffVolumeID  = MFUnset;
static int _MDInDischargeID     = MFUnset;
// Output
static int _MDOutDischLevel3ID  = MFUnset;
static int _MDOutRiverStorChgID = MFUnset;
static int _MDOutRiverStorageID = MFUnset;

static void _MDDischLevel3Accumulate (int itemID) {
// Input
	float runoff;     // Local runoff volume [m3/s]
	float discharge;  // Discharge from upstream [m3/s]

	runoff    = MFVarGetFloat(_MDInRunoffVolumeID, itemID, 0.0);
	discharge = MFVarGetFloat(_MDInDischargeID,    itemID, 0.0);

	MFVarSetFloat (_MDOutDischLevel3ID, itemID, discharge + runoff);
	MFVarSetFloat (_MDOutRiverStorChgID, itemID, 0.0);
	MFVarSetFloat (_MDOutRiverStorageID, itemID, 0.0);
}

int MDDischLevel3AccumulateDef () {

	if (_MDOutDischLevel3ID != MFUnset) return (_MDOutDischLevel3ID);

	MFDefEntering ("Discharge Level 3 - Accumulate");
	if (((_MDInRunoffVolumeID  = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDInDischargeID     = MFVarGetID (MDVarDischarge,       "m3/s", MFInput,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutDischLevel3ID  = MFVarGetID ("__DischLevel3",      "m3/s", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutRiverStorChgID = MFVarGetID (MDVarRiverStorageChg, "m3",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutRiverStorageID = MFVarGetID (MDVarRiverStorage,    "m3",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    (MFModelAddFunction (_MDDischLevel3Accumulate) == CMfailed)) return CMfailed;
	MFDefLeaving ("Discharge Accumulate");
	return (_MDOutDischLevel3ID);
}
