/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDAccumulate.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

//Input;
static int _MDInPrecipID     = MFUnset;

//Output
static int _MDOutAccPrecipID = MFUnset;

static void _MDAccumPrecip (int itemID) {
	float accum;
	
	accum = MFVarGetFloat(_MDInPrecipID, itemID, 0.0);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAccPrecipID, itemID, MFVarGetFloat (_MDOutAccPrecipID, itemID, 0.0) + accum);
}

int MDAccumPrecipDef() {

	if (_MDOutAccPrecipID != MFUnset) return (_MDOutAccPrecipID);

	MFDefEntering ("Accumulate Precipitation");
	if (((_MDInPrecipID     = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDOutAccPrecipID = MFVarGetID (MDVarAccPrecipitation, "m3/s",   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction(_MDAccumPrecip) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Precipitation");
	return (_MDOutAccPrecipID);	
}

//Input;
static int _MDInEvapID      = MFUnset;

//Output
static int _MDOutAccEvapID  = MFUnset;

static void _MDAccumEvap (int itemID) {
	float accum;
	
	accum = MFVarGetFloat(_MDInEvapID, itemID, 0.0);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAccEvapID, itemID, MFVarGetFloat (_MDOutAccEvapID, itemID, 0.0) + accum);
}

int MDAccumEvapDef() {

	if (_MDOutAccEvapID != MFUnset) return (_MDOutAccEvapID);

	MFDefEntering ("Accumulate Evapotranspiration");
	if (((_MDInEvapID     = MDEvapotranspirationDef ()) == CMfailed) ||
	    ((_MDOutAccEvapID = MFVarGetID (MDVarAccEvapotranspiration, "m3/s", MFRoute, MFState, MFBoundary))  == CMfailed) ||
	    (MFModelAddFunction(_MDAccumEvap) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Evapotranspiration");
	return (_MDOutAccEvapID);	
}

//Input;
static int _MDInSMoistChgID     = MFUnset;

//Output
static int _MDOutAccSMoistChgID = MFUnset;

static void _MDAccumSMoistChg (int itemID) {
	float accum;
	
	accum = MFVarGetFloat(_MDInSMoistChgID, itemID, 0.0);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAccSMoistChgID, itemID, MFVarGetFloat (_MDOutAccSMoistChgID, itemID, 0.0) + accum);
}

int MDAccumSMoistChgDef() {

	if (_MDOutAccSMoistChgID != MFUnset) return (_MDOutAccSMoistChgID);

	MFDefEntering ("Accumulate Soil Moisture Change");
	if (((_MDInSMoistChgID     = MDSoilMoistChgDef ()) == CMfailed) ||
	    ((_MDOutAccSMoistChgID = MFVarGetID (MDVarAccSoilMoistChange, "m3/s", MFRoute, MFState, MFBoundary))  == CMfailed) ||
	    (MFModelAddFunction(_MDAccumSMoistChg) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Soil Moisture Change");
	return (_MDOutAccSMoistChgID);	
}

//Input;
static int _MDInGrdWatChgID     = MFUnset;

//Output
static int _MDOutAccGrdWatChgID = MFUnset;

static void _MDAccumGrdWatChg (int itemID) {
	float accum;
	
	accum = MFVarGetFloat(_MDInGrdWatChgID, itemID, 0.0);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAccGrdWatChgID, itemID, MFVarGetFloat (_MDOutAccGrdWatChgID, itemID, 0.0) + accum);
}

int MDAccumGrdWatChgDef() {

	if (_MDOutAccGrdWatChgID != MFUnset) return (_MDOutAccGrdWatChgID);

	MFDefEntering ("Accumulate Groundwater Change");
	if (((_MDInGrdWatChgID     = MFVarGetID (MDVarGroundWaterChange,    "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutAccGrdWatChgID = MFVarGetID (MDVarAccGroundWaterChange, "m3/s", MFRoute,  MFState, MFBoundary))  == CMfailed) ||
	    (MFModelAddFunction (_MDAccumGrdWatChg) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Groundwater Change");
	return (_MDOutAccGrdWatChgID);	
}


//Input;
static int _MDInRunoffVolumeID  = MFUnset;

//Output
static int _MDOutAccRunoffID    = MFUnset;

static void _MDAccumRunoff (int itemID) {
	MFVarSetFloat(_MDOutAccRunoffID, itemID, MFVarGetFloat(_MDOutAccRunoffID,    itemID, 0.0) +
	                                         MFVarGetFloat (_MDInRunoffVolumeID, itemID, 0.0));
}

int MDAccumRunoffDef() {

	if (_MDOutAccRunoffID != MFUnset) return (_MDOutAccRunoffID);

	MFDefEntering ("Accumulate Runoff");
	if (((_MDInRunoffVolumeID  = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDOutAccRunoffID    = MFVarGetID (MDVarAccRunoff, "m3/s", MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDAccumRunoff) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Runoff");
	return (_MDOutAccRunoffID);	
}
