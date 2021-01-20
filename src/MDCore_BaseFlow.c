/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDBaseFlow.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInCore_RechargeID              = MFUnset;
static int _MDInIrrigation_GrossDemandID     = MFUnset;
static int _MDInIrrigation_ReturnFlowID      = MFUnset;
static int _MDInIrrigation_RunoffID          = MFUnset;
static int _MDInReservoir_FarmPondReleaseID  = MFUnset;

// Output
static int _MDOutCore_GrdWatID               = MFUnset;
static int _MDOutCore_GrdWatChgID            = MFUnset;
static int _MDOutCore_GrdWatRechargeID       = MFUnset;
static int _MDOutCore_GrdWatUptakeID         = MFUnset;
static int _MDOutCore_BaseFlowID             = MFUnset;
static int _MDOutCore_IrrUptakeGrdWaterID    = MFUnset;
static int _MDOutCore_Irrigation_UptakeExternalID = MFUnset;

static float _MDGroundWatBETA = 0.016666667;

static void _MDCore_BaseFlow (int itemID) {
// Input
	float irrDemand;               // Irrigation demand [mm/dt]
	float irrReturnFlow;           // Irrigational return flow [mm/dt]
    float irrRunoff;               // Irrigational runoff [mm/dt]
/// Output
	float grdWater;                // Groundwater size   [mm]
	float grdWaterChg;             // Groundwater change [mm/dt]
	float grdWaterRecharge;        // Groundwater recharge [mm/dt]
	float grdWaterUptake;          // Groundwater uptake [mm/dt]
	float baseFlow;                // Base flow from groundwater [mm/dt]
	float irrUptakeGrdWater = 0.0; // Irrigational water uptake from shallow groundwater [mm/dt]
	float irrUptakeExt      = 0.0; // Unmet irrigational water demand [mm/dt]
// Local
                     
	grdWaterChg = grdWater = MFVarGetFloat (_MDOutCore_GrdWatID, itemID, 0.0);
	if (grdWater < 0.0) grdWaterChg = grdWater = 0.0;			//RJS 071511
	grdWaterRecharge = MFVarGetFloat (_MDInCore_RechargeID, itemID, 0.0);
	grdWater = grdWater + grdWaterRecharge;

	if ((_MDInIrrigation_GrossDemandID != MFUnset) &&
	    (_MDInIrrigation_ReturnFlowID  != MFUnset)) {

		irrReturnFlow = MFVarGetFloat (_MDInIrrigation_ReturnFlowID,  itemID, 0.0);
        irrRunoff     = MFVarGetFloat (_MDInIrrigation_RunoffID,      itemID, 0.0);
		irrDemand     = MFVarGetFloat (_MDInIrrigation_GrossDemandID, itemID, 0.0);

		grdWater         = grdWater         + irrReturnFlow + irrRunoff;
		grdWaterRecharge = grdWaterRecharge + irrReturnFlow + irrRunoff;

		if (_MDInReservoir_FarmPondReleaseID != MFUnset) irrDemand = irrDemand - MFVarGetFloat(_MDInReservoir_FarmPondReleaseID,itemID,0.0);
		if (_MDOutCore_IrrUptakeGrdWaterID   != MFUnset) {
			if (irrDemand < grdWater) {
				// Irrigation demand is satisfied from groundwater storage 
				irrUptakeGrdWater = irrDemand;
				grdWater = grdWater - irrUptakeGrdWater;
			}
			else {
				// Irrigation demand needs external source
				irrUptakeGrdWater = grdWater;
				irrUptakeExt      = irrDemand - irrUptakeGrdWater;
				grdWater = 0.0;
			}
			MFVarSetFloat (_MDOutCore_IrrUptakeGrdWaterID, itemID, irrUptakeGrdWater);
		}
		else irrUptakeExt = irrDemand;
		MFVarSetFloat (_MDOutCore_Irrigation_UptakeExternalID, itemID, irrUptakeExt);
	}

	baseFlow    = grdWater * _MDGroundWatBETA;
	grdWater    = grdWater - baseFlow;
	grdWaterChg = grdWater - grdWaterChg;

	grdWaterUptake = baseFlow + irrUptakeGrdWater;

	MFVarSetFloat (_MDOutCore_GrdWatID,         itemID, grdWater);
    MFVarSetFloat (_MDOutCore_GrdWatChgID,      itemID, grdWaterChg);
    MFVarSetFloat (_MDOutCore_GrdWatRechargeID, itemID, grdWaterRecharge);
    MFVarSetFloat (_MDOutCore_GrdWatUptakeID,   itemID, grdWaterUptake);
	MFVarSetFloat (_MDOutCore_BaseFlowID,       itemID, baseFlow);
}

int MDCore_BaseFlowDef () {
	float par;
	const char *optStr;

	if (_MDOutCore_BaseFlowID != MFUnset) return (_MDOutCore_BaseFlowID);

	MFDefEntering ("Base flow");
	if (((optStr = MFOptionGet (MDParGroundWatBETA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDGroundWatBETA = par;

	if (((_MDInCore_RechargeID       = MDCore_RainInfiltrationDef()) == CMfailed) ||
        ((_MDInIrrigation_GrossDemandID = MDIrrigation_GrossDemandDef()) == CMfailed)) return (CMfailed);

	if ( _MDInIrrigation_GrossDemandID != MFUnset) {
		if (((_MDInReservoir_FarmPondReleaseID = MDReservoir_FarmPondReleaseDef ()) == CMfailed) ||
            ((_MDInIrrigation_ReturnFlowID     = MDIrrigation_ReturnFlowDef ())     == CMfailed) ||
            ((_MDInIrrigation_RunoffID         = MDIrrigation_RunoffDef ())         == CMfailed) ||
            ((_MDOutCore_IrrUptakeGrdWaterID   = MDIrrigation_UptakeGrdWaterDef ()) == CMfailed) ||
            ((_MDOutCore_Irrigation_UptakeExternalID = MFVarGetID (MDVarIrrigation_UptakeExternal, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed))
			return CMfailed;
	}
	if (((_MDOutCore_GrdWatID                = MFVarGetID (MDVarCore_GroundWater, "mm", MFOutput, MFState, MFInitial)) == CMfailed) ||
        ((_MDOutCore_GrdWatChgID             = MFVarGetID (MDVarCore_GroundWaterChange, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutCore_GrdWatRechargeID        = MFVarGetID (MDVarCore_GroundWaterRecharge, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutCore_GrdWatUptakeID          = MFVarGetID (MDVarCore_GroundWaterUptake, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutCore_BaseFlowID              = MFVarGetID (MDVarCore_BaseFlow, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction(_MDCore_BaseFlow) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Base flow ");
	return (_MDOutCore_BaseFlowID);
}
