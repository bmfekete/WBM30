/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDTempRiver.c

wil.wollheim@unh.edu

Calculate the temperature in runoff from the local grid cell.  Weight groundwater and surface water temperatures.

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInSurfCore_RunoffID          = MFUnset;
static int _MDInBaseFlowID            = MFUnset;
static int _MDInWTempSurfCore_RunoffID     = MFUnset;
static int _MDInWTempGrdWaterID       = MFUnset;

// Output
static int _MDOutWTempRiverID     = MFUnset;

static void _MDWTempRiver (int itemID) {
	float RechargeT;
	float GrdWaterT;
	float SurfaceRO;
	float GrdWaterRO;
	float TemperatureRO;

	SurfaceRO          = MFVarGetFloat (_MDInSurfCore_RunoffID,          itemID, 0.0);
	GrdWaterRO         = MFVarGetFloat (_MDInBaseFlowID,            itemID, 0.0);
	RechargeT          = MFVarGetFloat (_MDInWTempSurfCore_RunoffID,     itemID, 0.0);
	GrdWaterT          = MFVarGetFloat (_MDInWTempGrdWaterID,       itemID, 0.0);

	SurfaceRO  = MDMaximum(0, SurfaceRO);
	GrdWaterRO = MDMaximum(0, GrdWaterRO);

	TemperatureRO = MDMaximum((((SurfaceRO * RechargeT) + (GrdWaterRO * GrdWaterT)) / (SurfaceRO + GrdWaterRO)),0.0);
	MFVarSetFloat(_MDOutWTempRiverID,itemID,TemperatureRO);
}

int MDTP2M_WTempRiverDef () {

	if (_MDOutWTempRiverID != MFUnset) return (_MDOutWTempRiverID);

	MFDefEntering ("River temperature");

	if (((_MDInSurfCore_RunoffID      = MDCore_RainSurfRunoffDef()) == CMfailed) ||
        ((_MDInBaseFlowID        = MDCore_BaseFlowDef()) == CMfailed) ||
        ((_MDInWTempSurfCore_RunoffID = MDTP2M_WTempSurfRunoffDef()) == CMfailed) ||
        ((_MDInWTempGrdWaterID   = MDTP2M_WTempGrdWaterDef()) == CMfailed) ||
        ((_MDOutWTempRiverID     = MFVarGetID (MDVarTP2M_WTempRiver, "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDWTempRiver) == CMfailed)) return (CMfailed);

	MFDefLeaving ("River temperature");
	return (_MDOutWTempRiverID);
}
