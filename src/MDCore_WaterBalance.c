/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDWaterBalance.c

dominik.wisser@unh.edu
This is meant to check the vertical water balance for each grid cell. It does 
NOT include any water that is flowing laterally and should not be used to call BCG....
*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInCommon_PrecipID             = MFUnset;
static int _MDInEvaptrsID                   = MFUnset;
static int _MDInSnowPackChgID               = MFUnset;
static int _MDInSoilMoistChgID              = MFUnset;
static int _MDInAux_GrdWatChgID             = MFUnset;
static int _MDInCore_RunoffID               = MFUnset;
static int _MDInRouting_DischargeID         = MFUnset;

static int _MDInIrrEvapotranspID            = MFUnset;
static int _MDInIrrSoilMoistChgID           = MFUnset;
static int _MDInIrrigation_AreaFracID       = MFUnset;
static int _MDInIrrigation_GrossDemandID    = MFUnset;
static int _MDInIrrigation_ReturnFlowID     = MFUnset;
static int _MDInIrrigation_RunoffID         = MFUnset;
static int _MDInIrrigation_UptakeRiverID    = MFUnset;
static int _MDInIrrUptakeGrdWaterID         = MFUnset;
static int _MDInIrrigation_UptakeExcessID   = MFUnset;
static int _MDInReservoir_FarmPondReleaseID = MFUnset;
static int _MDInSmallResStorageChgID        = MFUnset;
static int _MDInSmallResEvapoID             = MFUnset;

// Output
static int _MDOutWaterBalanceID             = MFUnset;
static int _MDOutIrrUptakeBalanceID         = MFUnset;
static int _MDOutIrrWaterBalanceID          = MFUnset;

static void _MDWaterBalance(int itemID) {
// Input
	float precip       = MFVarGetFloat(_MDInCommon_PrecipID,         itemID, 0.0);
	float etp          = MFVarGetFloat(_MDInEvaptrsID,        itemID, 0.0);	
	float snowPackChg  = MFVarGetFloat(_MDInSnowPackChgID,    itemID, 0.0);	
	float soilMoistChg = MFVarGetFloat(_MDInSoilMoistChgID,   itemID, 0.0);
	float grdWaterChg  = MFVarGetFloat(_MDInAux_GrdWatChgID,      itemID, 0.0);
	float runoff       = MFVarGetFloat(_MDInCore_RunoffID,         itemID, 0.0);
	float irrAreaFrac        = 0.0;
	float irrGrossDemand     = 0.0;
    float irrReturnFlow      = 0.0;
	float irrRunoff          = 0.0;
	float irrEvapotransp     = 0.0;
	float irrSoilMoistChg    = 0.0;
	float irrUptakeGrdWater  = 0.0;
	float irrUptakeRiver     = 0.0;
	float irrUptakeExcess    = 0.0;
	float smallResStorageChg = 0.0;
	float smallResRelease    = 0.0;
//	float smallResEvapo      = 0.0;
// Output
	float balance;

	if (_MDInIrrigation_GrossDemandID != MFUnset) { 
		irrAreaFrac       = MFVarGetFloat (_MDInIrrigation_AreaFracID,            itemID, 0.0);
		irrGrossDemand    = MFVarGetFloat (_MDInIrrigation_GrossDemandID,         itemID, 0.0);
		irrReturnFlow     = MFVarGetFloat (_MDInIrrigation_ReturnFlowID,          itemID, 0.0);
        irrRunoff         = MFVarGetFloat (_MDInIrrigation_RunoffID,              itemID, 0.0);
		irrEvapotransp    = MFVarGetFloat (_MDInIrrEvapotranspID,         itemID, 0.0);
		irrSoilMoistChg   = _MDInIrrSoilMoistChgID        != MFUnset ? MFVarGetFloat (_MDInIrrSoilMoistChgID,        itemID, 0.0) : 0.0;
		irrUptakeGrdWater = _MDInIrrUptakeGrdWaterID      != MFUnset ? MFVarGetFloat (_MDInIrrUptakeGrdWaterID,      itemID, 0.0) : 0.0;
		irrUptakeRiver    = _MDInIrrigation_UptakeRiverID != MFUnset ? MFVarGetFloat (_MDInIrrigation_UptakeRiverID, itemID, 0.0) : 0.0;
		irrUptakeExcess   = MFVarGetFloat (_MDInIrrigation_UptakeExcessID,        itemID, 0.0);

		if (_MDInReservoir_FarmPondReleaseID != MFUnset) {
			smallResRelease    = MFVarGetFloat (_MDInReservoir_FarmPondReleaseID,    itemID, 0.0);
			smallResStorageChg = MFVarGetFloat (_MDInSmallResStorageChgID, itemID, 0.0);
//			smallResEvapo      = MFVarGetFloat (_MDInSmallResEvapoID,      itemID, 0.0);
		}

		balance = (precip - snowPackChg) * irrAreaFrac + irrGrossDemand - irrEvapotransp - irrSoilMoistChg - irrReturnFlow - irrRunoff;
		MFVarSetFloat (_MDOutIrrWaterBalanceID, itemID, balance);

		balance = irrGrossDemand - (irrUptakeGrdWater + irrUptakeRiver + irrUptakeExcess + smallResRelease);
		MFVarSetFloat (_MDOutIrrUptakeBalanceID, itemID, balance);
	}
	balance = precip + irrUptakeRiver + irrUptakeExcess - (etp + runoff + grdWaterChg + snowPackChg + soilMoistChg + smallResStorageChg);
	if (fabs (balance) > 0.01 )
	//	printf ("TIEM %i WaterBalance! %f precip %f Demand %f\n", itemID ,balance,precip,irrGrossDemand);

	MFVarSetFloat (_MDOutWaterBalanceID, itemID , balance);
}

int MDCore_WaterBalanceDef() {
 
	MFDefEntering ("WaterBalance");
	if ((MDAux_AccumBalanceDef() == CMfailed) ||
        ((_MDInCommon_PrecipID     = MDCommon_PrecipitationDef()) == CMfailed) ||
        ((_MDInRouting_DischargeID = MDRouting_DischargeDef()) == CMfailed) ||
        ((_MDInSnowPackChgID       = MDCore_SnowPackChgDef()) == CMfailed) ||
        ((_MDInSoilMoistChgID      = MDCore_SoilMoistChgDef()) == CMfailed) ||
        ((_MDInCore_RunoffID       = MDCore_RunoffDef()) == CMfailed) ||
        ((_MDInEvaptrsID           = MFVarGetID (MDVarCore_Evapotranspiration, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDInAux_GrdWatChgID     = MFVarGetID (MDVarCore_GroundWaterChange, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutWaterBalanceID     = MFVarGetID (MDVarCore_WaterBalance, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction(_MDWaterBalance) == CMfailed))
	    return (CMfailed);
	if ((_MDInIrrigation_GrossDemandID       = MDIrrigation_GrossDemandDef()) != MFUnset) {
		if ((_MDInIrrigation_GrossDemandID == CMfailed) ||
            ((_MDInIrrigation_UptakeRiverID  = MDIrrigation_UptakeRiverDef()) == CMfailed) ||
            ((_MDInIrrUptakeGrdWaterID       = MDIrrigation_UptakeGrdWaterDef()) == CMfailed) ||
            ((_MDInIrrSoilMoistChgID         = MDIrrigation_SoilMoistChgDef()) == CMfailed) ||
            ((_MDInIrrigation_AreaFracID     = MDIrrigation_IrrAreaDef()) == CMfailed) ||
            ((_MDInIrrEvapotranspID          = MFVarGetID (MDVarIrrigation_Evapotranspiration, "mm", MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
            ((_MDInIrrigation_ReturnFlowID   = MFVarGetID (MDVarIrrigation_ReturnFlow,         "mm", MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
            ((_MDInIrrigation_RunoffID       = MFVarGetID (MDVarIrrigation_Runoff,             "mm", MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
            ((_MDInIrrigation_UptakeExcessID = MFVarGetID (MDVarIrrigation_UptakeExcess,       "mm", MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
            ((_MDOutIrrUptakeBalanceID       = MFVarGetID (MDVarIrrigation_UptakeBalance,      "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
            ((_MDOutIrrWaterBalanceID        = MFVarGetID (MDVarIrrigation_WaterBalance,       "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed))
	    	return (CMfailed);		
		if ((_MDInReservoir_FarmPondReleaseID        = MDReservoir_FarmPondReleaseDef()) != MFUnset) {
			if (( _MDInReservoir_FarmPondReleaseID == CMfailed) ||
                ((_MDInSmallResEvapoID      = MFVarGetID (MDVarReservoir_FarmPondEvaporation, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
                ((_MDInSmallResStorageChgID = MFVarGetID (MDVarReservoir_FarmPondStorageChange, "mm", MFInput, MFState, MFInitial)) == CMfailed))
			    return (CMfailed);
		}
	}

	MFDefLeaving ("WaterBalance");
	return (_MDOutWaterBalanceID);	
}
