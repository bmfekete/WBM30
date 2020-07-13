/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDAccBalance.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

//Input;
static int _MDInAccPrecipID    = MFUnset;
static int _MDInAccEvapID      = MFUnset;
static int _MDInAccSMoistChgID = MFUnset;
static int _MDInAccGrdWatChgID = MFUnset;
static int _MDInAccRunoffID    = MFUnset;

//Output
static int _MDOutAccBalanceID  = MFUnset;

static void _MDAccumBalance (int itemID)
{
// Input
	double precip;    // Precipitation [mm/dt]
	double evap;      // Evapotranspiration [mm/dt]
	double sMoistChg; // Soil moisture change [mm/dt]
	double grdWatChg; // Groundwater change [mm/dt]
	double runoff;    // Runoff [mm/dt]

	if (MFVarTestMissingVal (_MDInAccPrecipID,    itemID) ||
	    MFVarTestMissingVal (_MDInAccEvapID,      itemID) ||
		MFVarTestMissingVal (_MDInAccSMoistChgID, itemID) ||
		MFVarTestMissingVal (_MDInAccGrdWatChgID, itemID) ||
		MFVarTestMissingVal (_MDInAccRunoffID,    itemID)) MFVarSetMissingVal (_MDOutAccBalanceID,itemID);
	else {
		precip     = MFVarGetFloat(_MDInAccPrecipID,    itemID, 0.0);
		evap       = MFVarGetFloat(_MDInAccEvapID,      itemID, 0.0);
		sMoistChg  = MFVarGetFloat(_MDInAccSMoistChgID, itemID, 0.0);
		grdWatChg  = MFVarGetFloat(_MDInAccGrdWatChgID, itemID, 0.0);
		runoff     = MFVarGetFloat(_MDInAccRunoffID,    itemID, 0.0);
		MFVarSetFloat(_MDOutAccBalanceID, itemID, precip + evap + sMoistChg + grdWatChg + runoff);
	}
}

int MDAccumBalanceDef() {

	if (_MDOutAccBalanceID != MFUnset) return (_MDOutAccBalanceID);

	MFDefEntering ("Accumulated Balance");

	if (((_MDInAccPrecipID    = MDAccumPrecipDef())    == CMfailed) ||
	    ((_MDInAccRunoffID    = MDAccumRunoffDef())    == CMfailed) ||
	    ((_MDInAccGrdWatChgID = MDAccumGrdWatChgDef()) == CMfailed) ||
	    ((_MDInAccSMoistChgID = MDAccumSMoistChgDef()) == CMfailed) ||
	    ((_MDInAccEvapID      = MDAccumEvapDef())      == CMfailed) ||
	    ((_MDOutAccBalanceID  = MFVarGetID (MDVarAccBalance,  "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction(_MDAccumBalance) == CMfailed)) return CMfailed;

	MFDefLeaving ("Accumulated Balance");
	return (_MDOutAccBalanceID);	
}
