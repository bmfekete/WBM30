/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDichLevel3Muskingum.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInMuskingumC0ID   = MFUnset;
static int _MDInMuskingumC1ID   = MFUnset;
static int _MDInMuskingumC2ID   = MFUnset;
static int _MDInRunoffVolumeID  = MFUnset;
static int _MDInDischargeID     = MFUnset;
// Output
static int _MDOutDischAux0ID    = MFUnset;
static int _MDOutDischAux1ID    = MFUnset;
static int _MDOutDischLevel3ID  = MFUnset;
static int _MDOutRiverStorChgID = MFUnset;
static int _MDOutRiverStorageID = MFUnset;

static void _MDDischLevel3Muskingum (int itemID) {
// Input
	float C0;              // Muskingum C0 coefficient (current inflow)
	float C1;              // Muskingum C1 coefficient (previous inflow)
	float C2;              // MUskingum C2 coefficient (previous outflow) 
	float runoff;          // Runoff [mm/dt]
// Output
	float inDischCurrent;  // Upstream discharge at the current time step [m3/s]
	float outDisch;        // Downstream discharge [m3/s]
// Local
	float inDischPrevious; // Upstream discharge at the previous time step [m3/s]
	float storChg;         // River Storage Change [m3]
	float storage;         // River Storage [m3]
	
	C0 = MFVarGetFloat (_MDInMuskingumC0ID,   itemID, 1.0);
	C1 = MFVarGetFloat (_MDInMuskingumC1ID,   itemID, 0.0);
	C2 = MFVarGetFloat (_MDInMuskingumC2ID,   itemID, 0.0);

	runoff          = MFVarGetFloat (_MDInRunoffVolumeID,  itemID, 0.0);
 	inDischPrevious = MFVarGetFloat (_MDOutDischAux0ID,    itemID, 0.0);
	outDisch        = MFVarGetFloat (_MDOutDischAux1ID,    itemID, 0.0);
	inDischCurrent  = MFVarGetFloat (_MDInDischargeID,     itemID, 0.0) + runoff;
	storage         = MFVarGetFloat (_MDOutRiverStorageID, itemID, 0.0);

	outDisch = C0 * inDischCurrent + C1 * inDischPrevious + C2 * outDisch;
	storChg  = inDischCurrent - outDisch;
	storage = storage + storChg > 0.0 ? storage + storChg : 0.0;

	MFVarSetFloat (_MDOutDischAux0ID,    itemID, inDischCurrent);
	MFVarSetFloat (_MDOutDischAux1ID,    itemID, outDisch);
	MFVarSetFloat (_MDOutDischLevel3ID,  itemID, outDisch);
	MFVarSetFloat (_MDOutRiverStorChgID, itemID, storChg);
	MFVarSetFloat (_MDOutRiverStorageID, itemID, storage);
}

int MDDischLevel3MuskingumDef () {

	if (_MDOutDischLevel3ID != MFUnset) return (_MDOutDischLevel3ID);

	MFDefEntering ("Discharge Muskingum");

	if (((_MDInRunoffVolumeID  = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDInMuskingumC0ID   = MDDischLevel3MuskingumCoeffDef ()) == CMfailed) ||
	    ((_MDInMuskingumC1ID   = MFVarGetID (MDVarMuskingumC1,      MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInMuskingumC2ID   = MFVarGetID (MDVarMuskingumC2,      MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInDischargeID     = MFVarGetID (MDVarDischarge,        "m3/s",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutDischAux0ID    = MFVarGetID (MDVarDischarge0,       "m3/s",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutDischAux1ID    = MFVarGetID (MDVarDischarge1,       "m3/s",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutDischLevel3ID  = MFVarGetID ("__DischLevel3",       "m3/s",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutRiverStorChgID = MFVarGetID (MDVarRiverStorageChg,  "m3",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutRiverStorageID = MFVarGetID (MDVarRiverStorage,     "m3",     MFOutput, MFState, MFInitial))  == CMfailed) ||
	    (MFModelAddFunction(_MDDischLevel3Muskingum) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Discharge Muskingum");
	return (_MDOutDischLevel3ID);
}

