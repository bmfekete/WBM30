/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDAux_AccBalance.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

//Input;
static int _MDInAux_AccPrecipID    = MFUnset;
static int _MDInAux_AccEvapID      = MFUnset;
static int _MDInAux_AccSMoistChgID = MFUnset;
static int _MDInAux_AccGrdWatChgID = MFUnset;
static int _MDInAux_AccCore_RunoffID    = MFUnset;

//Output
static int _MDOutAux_AccBalanceID  = MFUnset;

static void _MDAux_AccumBalance (int itemID)
{
// Input
	double precip;    // Precipitation [mm/dt]
	double evap;      // Evapotranspiration [mm/dt]
	double sMoistChg; // Soil moisture change [mm/dt]
	double grdWatChg; // Groundwater change [mm/dt]
	double runoff;    // Runoff [mm/dt]

	if (MFVarTestMissingVal (_MDInAux_AccPrecipID, itemID) ||
        MFVarTestMissingVal (_MDInAux_AccEvapID, itemID) ||
        MFVarTestMissingVal (_MDInAux_AccSMoistChgID, itemID) ||
        MFVarTestMissingVal (_MDInAux_AccGrdWatChgID, itemID) ||
        MFVarTestMissingVal (_MDInAux_AccCore_RunoffID, itemID)) MFVarSetMissingVal (_MDOutAux_AccBalanceID, itemID);
	else {
		precip     = MFVarGetFloat(_MDInAux_AccPrecipID, itemID, 0.0);
		evap       = MFVarGetFloat(_MDInAux_AccEvapID, itemID, 0.0);
		sMoistChg  = MFVarGetFloat(_MDInAux_AccSMoistChgID, itemID, 0.0);
		grdWatChg  = MFVarGetFloat(_MDInAux_AccGrdWatChgID, itemID, 0.0);
		runoff     = MFVarGetFloat(_MDInAux_AccCore_RunoffID, itemID, 0.0);
		MFVarSetFloat(_MDOutAux_AccBalanceID, itemID, precip + evap + sMoistChg + grdWatChg + runoff);
	}
}

int MDAux_AccumBalanceDef() {

	if (_MDOutAux_AccBalanceID != MFUnset) return (_MDOutAux_AccBalanceID);

	MFDefEntering ("Accumulated Balance");

	if (((_MDInAux_AccPrecipID    = MDAux_AccumPrecipDef()) == CMfailed) ||
        ((_MDInAux_AccCore_RunoffID    = MDAux_AccumRunoffDef()) == CMfailed) ||
        ((_MDInAux_AccGrdWatChgID = MDAux_AccumGrdWatChgDef()) == CMfailed) ||
        ((_MDInAux_AccSMoistChgID = MDAux_AccumSMoistChgDef()) == CMfailed) ||
        ((_MDInAux_AccEvapID      = MDAux_AccumEvapDef()) == CMfailed) ||
        ((_MDOutAux_AccBalanceID  = MFVarGetID (MDVarAux_AccBalance, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction(_MDAux_AccumBalance) == CMfailed)) return CMfailed;

	MFDefLeaving ("Accumulated Balance");
	return (_MDOutAux_AccBalanceID);
}
