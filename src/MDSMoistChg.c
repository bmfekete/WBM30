/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDSMoistChg.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainSoilMoistID     = MFUnset;
static int _MDInRainSoilMoistChgID  = MFUnset;
static int _MDInIrrSoilMoistID      = MFUnset;
static int _MDInIrrSoilMoistChgID   = MFUnset;
static int _MDInSoilAvailWaterCapID = MFUnset;
// Output
static int _MDOutSoilMoistID        = MFUnset;
static int _MDOutSoilMoistChgID     = MFUnset;
static int _MDOutRelSoilMoistID     = MFUnset;

static void _MDSoilMoistChg (int itemID) {	
// Input
	float sMoist;        // Non-irrigated soil moisture [mm/dt]
	float sMoistChg;     // Non-irrigated soil moisture change [mm/dt]
	float soilAvailWaterCap; // Available water capacity [mm]
	
	sMoist    = MFVarGetFloat (_MDInRainSoilMoistID,     itemID, 0.0)
	          + (_MDInIrrSoilMoistID    != MFUnset ? MFVarGetFloat (_MDInIrrSoilMoistID,    itemID, 0.0) : 0.0);
	sMoistChg = MFVarGetFloat (_MDInRainSoilMoistChgID,  itemID, 0.0)
	          + (_MDInIrrSoilMoistChgID != MFUnset ? MFVarGetFloat (_MDInIrrSoilMoistChgID, itemID, 0.0) : 0.0);
	soilAvailWaterCap = MFVarGetFloat (_MDInSoilAvailWaterCapID, itemID, 0.0);
 //   printf("soilAvailWaterCap = %f\n", soilAvailWaterCap);
	MFVarSetFloat (_MDOutSoilMoistID,    itemID, sMoist);
	MFVarSetFloat (_MDOutSoilMoistChgID, itemID, sMoistChg);
	MFVarSetFloat (_MDOutRelSoilMoistID, itemID, CMmathEqualValues (soilAvailWaterCap, 0.0) ? 0.0 : sMoist / soilAvailWaterCap);
}

int MDSoilMoistChgDef () {
	int ret;
	if (_MDOutSoilMoistChgID != MFUnset) return (_MDOutSoilMoistChgID);

	MFDefEntering ("Soil Moisture");

	if (((ret = MDIrrGrossDemandDef ()) != MFUnset) &&
	    ((ret == CMfailed) ||
	     ((_MDInIrrSoilMoistID     = MDIrrSoilMoistureDef ()) == CMfailed) ||
	     ((_MDInIrrSoilMoistChgID  = MDIrrSoilMoistChgDef ()) == CMfailed)))
	     return (CMfailed);
	if (((_MDInSoilAvailWaterCapID = MDSoilAvailWaterCapDef ()) == CMfailed) ||
	    ((_MDInRainSoilMoistID     = MFVarGetID (MDVarRainSoilMoisture,    "mm",   MFInput,  MFState, MFInitial))  == CMfailed) ||
        ((_MDInRainSoilMoistChgID  = MFVarGetID (MDVarRainSoilMoistChange, "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSoilMoistID        = MFVarGetID (MDVarSoilMoisture,        "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutSoilMoistChgID     = MFVarGetID (MDVarSoilMoistChange,     "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutRelSoilMoistID     = MFVarGetID (MDVarRelSoilMoisture,     "mm",   MFOutput, MFState, MFInitial)) == CMfailed) ||
       (MFModelAddFunction (_MDSoilMoistChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Soil Moisture");
	return (_MDOutSoilMoistChgID);
}
