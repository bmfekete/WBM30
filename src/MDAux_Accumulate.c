/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDAux_Accumulate.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

//Input;
static int _MDInCommon_PrecipID     = MFUnset;
//Output
static int _MDOutAux_AccPrecipID = MFUnset;

static void _MDAux_AccumPrecip (int itemID) {
	float accum;
	
	accum = MFVarGetFloat(_MDInCommon_PrecipID, itemID, 0.0);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAux_AccPrecipID, itemID, MFVarGetFloat (_MDOutAux_AccPrecipID, itemID, 0.0) + accum);
}

int MDAux_AccumPrecipDef() {

	if (_MDOutAux_AccPrecipID != MFUnset) return (_MDOutAux_AccPrecipID);

	MFDefEntering ("Accumulate Precipitation");
	if (((_MDInCommon_PrecipID     = MDCommon_PrecipitationDef()) == CMfailed) ||
        ((_MDOutAux_AccPrecipID = MFVarGetID (MDVarAux_AccPrecipitation, "m3/s", MFRoute, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction(_MDAux_AccumPrecip) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Precipitation");
	return (_MDOutAux_AccPrecipID);	
}

//Input;
static int _MDInAux_EvapID      = MFUnset;
//Output
static int _MDOutAux_AccEvapID  = MFUnset;

static void _MDAux_AccumEvap (int itemID) {
	float accum;
	
	accum = MFVarGetFloat(_MDInAux_EvapID, itemID, 0.0);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAux_AccEvapID, itemID, MFVarGetFloat (_MDOutAux_AccEvapID, itemID, 0.0) + accum);
}

int MDAux_AccumEvapDef() {

	if (_MDOutAux_AccEvapID != MFUnset) return (_MDOutAux_AccEvapID);

	MFDefEntering ("Accumulate Evapotranspiration");
	if (((_MDInAux_EvapID     = MDCore_EvapotranspirationDef()) == CMfailed) ||
        ((_MDOutAux_AccEvapID = MFVarGetID (MDVarAux_AccEvapotranspiration, "m3/s", MFRoute, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction(_MDAux_AccumEvap) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Evapotranspiration");
	return (_MDOutAux_AccEvapID);
}

//Input;
static int _MDInAux_SMoistChgID     = MFUnset;
//Output
static int _MDOutAux_AccSMoistChgID = MFUnset;

static void _MDAux_AccumSMoistChg (int itemID) {
	float accum;
	
	accum = MFVarGetFloat(_MDInAux_SMoistChgID, itemID, 0.0);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAux_AccSMoistChgID, itemID, MFVarGetFloat (_MDOutAux_AccSMoistChgID, itemID, 0.0) + accum);
}

int MDAux_AccumSMoistChgDef() {

	if (_MDOutAux_AccSMoistChgID != MFUnset) return (_MDOutAux_AccSMoistChgID);

	MFDefEntering ("Accumulate Soil Moisture Change");
	if (((_MDInAux_SMoistChgID     = MDCore_SoilMoistChgDef()) == CMfailed) ||
        ((_MDOutAux_AccSMoistChgID = MFVarGetID (MDVarAux_AccSoilMoistChange, "m3/s", MFRoute, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction(_MDAux_AccumSMoistChg) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Soil Moisture Change");
	return (_MDOutAux_AccSMoistChgID);
}

//Input;
static int _MDInAux_GrdWatChgID     = MFUnset;
//Output
static int _MDOutAux_AccGrdWatChgID = MFUnset;

static void _MDAux_AccumGrdWatChg (int itemID) {
	float accum;
	
	accum = MFVarGetFloat(_MDInAux_GrdWatChgID, itemID, 0.0);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAux_AccGrdWatChgID, itemID, MFVarGetFloat (_MDOutAux_AccGrdWatChgID, itemID, 0.0) + accum);
}

int MDAux_AccumGrdWatChgDef() {

	if (_MDOutAux_AccGrdWatChgID != MFUnset) return (_MDOutAux_AccGrdWatChgID);

	MFDefEntering ("Accumulate Groundwater Change");
	if (((_MDInAux_GrdWatChgID     = MFVarGetID (MDVarCore_GroundWaterChange, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutAux_AccGrdWatChgID = MFVarGetID (MDVarAux_AccGroundWaterChange, "m3/s", MFRoute, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction(_MDAux_AccumGrdWatChg) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Groundwater Change");
	return (_MDOutAux_AccGrdWatChgID);	
}


//Input;
static int _MDInAux_RunoffVolumeID  = MFUnset;
//Output
static int _MDOutAux_AccCore_RunoffID    = MFUnset;

static void _MDAux_AccumRunoff (int itemID) {
	MFVarSetFloat(_MDOutAux_AccCore_RunoffID, itemID, MFVarGetFloat(_MDOutAux_AccCore_RunoffID,    itemID, 0.0) +
	                                         MFVarGetFloat (_MDInAux_RunoffVolumeID, itemID, 0.0));
}

int MDAux_AccumRunoffDef() {

	if (_MDOutAux_AccCore_RunoffID != MFUnset) return (_MDOutAux_AccCore_RunoffID);

	MFDefEntering ("Accumulate Runoff");
	if (((_MDInAux_RunoffVolumeID  = MDCore_RunoffVolumeDef()) == CMfailed) ||
        ((_MDOutAux_AccCore_RunoffID    = MFVarGetID (MDVarAux_AccRunoff, "m3/s", MFRoute, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction(_MDAux_AccumRunoff) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Accumulate Runoff");
	return (_MDOutAux_AccCore_RunoffID);	
}
