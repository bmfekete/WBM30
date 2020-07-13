/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDWaterBalance.c

dominik.wisser@unh.edu
This is meant to check the vertical water balance for each grid cell. It does 
NOT include any water that is flowing laterally and should not be used to call BCG....
*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInPrecipID             = MFUnset;
static int _MDInEvaptrsID            = MFUnset;
static int _MDInSnowPackChgID        = MFUnset;
static int _MDInSoilMoistChgID       = MFUnset;
static int _MDInGrdWatChgID          = MFUnset;
static int _MDInRunoffID             = MFUnset;
static int _MDInDischargeID          = MFUnset;

static int _MDInIrrEvapotranspID     = MFUnset;
static int _MDInIrrSoilMoistChgID    = MFUnset;
static int _MDInIrrAreaFracID        = MFUnset;
static int _MDInIrrGrossDemandID     = MFUnset;
static int _MDInIrrReturnFlowID      = MFUnset;
static int _MDInIrrUptakeRiverID     = MFUnset;
static int _MDInIrrUptakeGrdWaterID  = MFUnset;
static int _MDInIrrUptakeExcessID    = MFUnset;
static int _MDInSmallResReleaseID    = MFUnset;
static int _MDInSmallResStorageChgID = MFUnset;
static int _MDInSmallResEvapoID      = MFUnset;

// Output
static int _MDOutWaterBalanceID      = MFUnset;
static int _MDOutIrrUptakeBalanceID  = MFUnset;
static int _MDOutIrrWaterBalanceID   = MFUnset;

static void _MDWaterBalance(int itemID) {
// Input
	float precip       = MFVarGetFloat(_MDInPrecipID,         itemID, 0.0);
	float etp          = MFVarGetFloat(_MDInEvaptrsID,        itemID, 0.0);	
	float snowPackChg  = MFVarGetFloat(_MDInSnowPackChgID,    itemID, 0.0);	
	float soilMoistChg = MFVarGetFloat(_MDInSoilMoistChgID,   itemID, 0.0);
	float grdWaterChg  = MFVarGetFloat(_MDInGrdWatChgID,      itemID, 0.0);
	float runoff       = MFVarGetFloat(_MDInRunoffID,         itemID, 0.0);
	float irrAreaFrac        = 0.0;
	float irrGrossDemand     = 0.0;
	float irrReturnFlow      = 0.0;
	float irrEvapotransp     = 0.0;
	float irrSoilMoistChg    = 0.0;
	float irrUptakeGrdWater  = 0.0;
	float irrUptakeRiver     = 0.0;
	float irrUptakeExcess    = 0.0;
	float smallResStorageChg = 0.0;
	float smallResRelease    = 0.0;
	float smallResEvapo      = 0.0;
// Output
	float balance;

	if (_MDInIrrGrossDemandID != MFUnset) { 
		irrAreaFrac       = MFVarGetFloat (_MDInIrrAreaFracID,            itemID, 0.0);
		irrGrossDemand    = MFVarGetFloat (_MDInIrrGrossDemandID,         itemID, 0.0);
		irrReturnFlow     = MFVarGetFloat (_MDInIrrReturnFlowID,          itemID, 0.0);
		irrEvapotransp    = MFVarGetFloat (_MDInIrrEvapotranspID,         itemID, 0.0);
		irrSoilMoistChg   = _MDInIrrSoilMoistChgID   != MFUnset ? MFVarGetFloat (_MDInIrrSoilMoistChgID,   itemID, 0.0) : 0.0;
		irrUptakeGrdWater = _MDInIrrUptakeGrdWaterID != MFUnset ? MFVarGetFloat (_MDInIrrUptakeGrdWaterID, itemID, 0.0) : 0.0;
		irrUptakeRiver    = _MDInIrrUptakeRiverID    != MFUnset ? MFVarGetFloat (_MDInIrrUptakeRiverID,    itemID, 0.0) : 0.0;
		irrUptakeExcess   = MFVarGetFloat (_MDInIrrUptakeExcessID,        itemID, 0.0);

		if (_MDInSmallResReleaseID != MFUnset) {
			smallResRelease    = MFVarGetFloat (_MDInSmallResReleaseID,    itemID, 0.0);
			smallResStorageChg = MFVarGetFloat (_MDInSmallResStorageChgID, itemID, 0.0);
			smallResEvapo      = MFVarGetFloat (_MDInSmallResEvapoID,      itemID, 0.0);
		}

		balance = (precip - snowPackChg) * irrAreaFrac + irrGrossDemand - irrEvapotransp - irrSoilMoistChg - irrReturnFlow;
		MFVarSetFloat (_MDOutIrrWaterBalanceID, itemID, balance);

		balance = irrGrossDemand - (irrUptakeGrdWater + irrUptakeRiver + irrUptakeExcess + smallResRelease);
		MFVarSetFloat (_MDOutIrrUptakeBalanceID, itemID, balance);
	}
	balance = precip + irrUptakeRiver + irrUptakeExcess - (etp + runoff + grdWaterChg + snowPackChg + soilMoistChg + smallResStorageChg);
	if (fabs (balance) > 0.01 )
	//	printf ("TIEM %i WaterBalance! %f precip %f Demand %f\n", itemID ,balance,precip,irrGrossDemand);
		

	MFVarSetFloat (_MDOutWaterBalanceID, itemID , balance);
}

int MDWaterBalanceDef() {
 
	MFDefEntering ("WaterBalance");
	if ((                                  MDAccumBalanceDef     ()  == CMfailed) ||
	    ((_MDInPrecipID                  = MDPrecipitationDef    ()) == CMfailed) ||
	    ((_MDInDischargeID               = MDDischargeDef        ()) == CMfailed) ||
	 
	    ((_MDInSnowPackChgID             = MDSPackChgDef         ()) == CMfailed) ||
	    ((_MDInSoilMoistChgID            = MDSoilMoistChgDef     ()) == CMfailed) ||
	    ((_MDInRunoffID                  = MDRunoffDef           ()) == CMfailed) ||
	    ((_MDInEvaptrsID                 = MFVarGetID (MDVarEvapotranspiration,      "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInGrdWatChgID               = MFVarGetID (MDVarGroundWaterChange,       "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutWaterBalanceID           = MFVarGetID (MDVarWaterBalance,            "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction(_MDWaterBalance) == CMfailed))
	    return (CMfailed);
	if ((_MDInIrrGrossDemandID           = MDIrrGrossDemandDef    ()) != MFUnset) {
		if ((_MDInIrrGrossDemandID == CMfailed) ||
	        ((_MDInIrrUptakeRiverID      = MDIrrUptakeRiverDef    ()) == CMfailed) ||
	        ((_MDInIrrUptakeGrdWaterID   = MDIrrUptakeGrdWaterDef ()) == CMfailed) ||
	        ((_MDInIrrSoilMoistChgID     = MDIrrSoilMoistChgDef   ()) == CMfailed) ||
	        ((_MDInIrrAreaFracID         = MDIrrigatedAreaDef    ())==  CMfailed) ||
	        ((_MDInIrrEvapotranspID      = MFVarGetID (MDVarIrrEvapotranspiration, "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	        ((_MDInIrrReturnFlowID       = MFVarGetID (MDVarIrrReturnFlow,         "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) || 
	        ((_MDInIrrUptakeExcessID     = MFVarGetID (MDVarIrrUptakeExcess,       "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
		    ((_MDOutIrrUptakeBalanceID   = MFVarGetID (MDVarIrrUptakeBalance,      "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
		    ((_MDOutIrrWaterBalanceID    = MFVarGetID (MDVarIrrWaterBalance,       "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed))
	    	return (CMfailed);		
		if ((_MDInSmallResReleaseID        = MDSmallReservoirReleaseDef ()) != MFUnset) {
			if (( _MDInSmallResReleaseID == CMfailed) ||
			    ((_MDInSmallResEvapoID      = MFVarGetID (MDVarSmallResEvaporation,   "mm", MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDInSmallResStorageChgID = MFVarGetID (MDVarSmallResStorageChange, "mm", MFInput, MFState, MFInitial))  == CMfailed))
			    return (CMfailed);
		}
	}
	MFDefLeaving ("WaterBalance");
	return (_MDOutWaterBalanceID);	
}
