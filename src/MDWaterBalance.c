/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDWaterBalance.c

dominik.wisser@unh.edu
This is meant to check the vertical water balance for each grid cell. It does 
NOT include any water that is flowing laterally and should not be used to call BCG....
*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInPrecipID             = MFUnset;
static int _MDInEvaptrsID            = MFUnset;
static int _MDInPetID                = MFUnset;
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

// Surface Runoff Pool
static int _MDInRunoffPoolChgID		 = MFUnset;
static int _MDInRunoffPoolID		 = MFUnset;
static int _MDInRunoffPoolRechargeID = MFUnset;
static int _MDInRunoffPoolReleaseID	 = MFUnset;

// Storm Runoff
static int _MDInStormRunoffImpID 	 = MFUnset;
static int _MDInStormRunoffH2OID 	 = MFUnset;
static int _MDInPrecipPervID  		 = MFUnset;
static int _MDInStormRunoffTotalID   = MFUnset;
static int _MDInRunofftoPervID 	 	 = MFUnset;

// Soil Moisture
static int _MDInEvaptrsNotScaledID      = MFUnset;
static int _MDInSoilMoistNotScaledID    = MFUnset;
static int _MDInSoilMoistID        	    = MFUnset;
static int _MDInSoilAvailWaterCapID     = MFUnset;
static int _MDInImpFractionID           = MFUnset;
static int _MDInH2OFractionID		    = MFUnset;
static int _MDInExcessID			    = MFUnset;
static int _MDInExcessNotScaledID	    = MFUnset;
static int _MDInSoilMoistChgNotScaledID = MFUnset;

// Rain Infiltration
static int _MDInRainWaterSurplusID  = MFUnset;
static int _MDInRainSurfRunoffID   = MFUnset;
static int _MDInRainInfiltrationID = MFUnset;

// Groundwater
static int _MDInGrdWatID                = MFUnset;
static int _MDInGrdWatRechargeID        = MFUnset;
static int _MDInBaseFlowID              = MFUnset;

// Snowpack
static int _MDInSnowFallID    = MFUnset;
static int _MDInSnowMeltID    = MFUnset;
static int _MDInSnowPackID    = MFUnset;

// Runoff
static int _MDInTotalSurfRunoffID  = MFUnset;

// Routing
static int _MDInRunoffVolumeID  = MFUnset;
static int _MDInQPreID          = MFUnset;
static int _MDInQCurID          = MFUnset;
static int _MDInQOutID          = MFUnset;
static int _MDInRiverStorageID  = MFUnset;
static int _MDInMuskingumC0ID   = MFUnset;
static int _MDInMuskingumC1ID   = MFUnset;
static int _MDInMuskingumC2ID   = MFUnset;
static int _MDInRiverStorChgID  = MFUnset;
static int _MDInDischRJSID      = MFUnset;

static int _MDInPropROStormWaterID     = MFUnset;
static int _MDInPropROSurfaceWaterID   = MFUnset;
static int _MDInPropROGroundWaterID    = MFUnset;    
static int _MDInQStormWaterID      = MFUnset;
static int _MDInQSurfaceWaterID    = MFUnset;
static int _MDInQGroundWaterID     = MFUnset;    
static int _MDInPropRSStormWaterID     = MFUnset;
static int _MDInPropRSSurfaceWaterID   = MFUnset;
static int _MDInPropRSGroundWaterID    = MFUnset;
static int _MDInPropQinStormWaterID    = MFUnset;
static int _MDInPropQinSurfaceWaterID  = MFUnset;
static int _MDInPropQinGroundWaterID   = MFUnset;    
static int _MDInPropRSinStormWaterID   = MFUnset;
static int _MDInPropRSinSurfaceWaterID = MFUnset;
static int _MDInPropRSinGroundWaterID  = MFUnset;

 static int _MDInPropROFlagID          = MFUnset;
 static int _MDInPropQinFlagID         = MFUnset;
 static int _MDInPropRSinFlagID        = MFUnset;
 static int _MDInQFlagID               = MFUnset;
 static int _MDInPropRSFlagID          = MFUnset;

/*
// DIN  
static int _MDInDischarge0ID         = MFUnset;
static int _MDInLocalLoad_DINID      = MFUnset;
static int _MDStoreWater_DINID       = MFUnset;
static int _MDStoreWaterMixing_DINID = MFUnset;
static int _MDWTemp_QxTID            = MFUnset;
static int _MDInPostConc_DINID       = MFUnset;
static int _MDInPreConc_DINID        = MFUnset;
static int _MDInConcMixing_DINID     = MFUnset;
static int _MDFlux_DINID             = MFUnset;
*/
// Output
static int _MDOutWaterBalanceID      = MFUnset;
static int _MDOutIrrUptakeBalanceID  = MFUnset;
static int _MDOutIrrWaterBalanceID   = MFUnset;

static void _MDWaterBalance(int itemID) {
// Input
	float precip        	 = MFVarGetFloat(_MDInPrecipID,         itemID, 0.0);
	float etp           	 = MFVarGetFloat(_MDInEvaptrsID,        itemID, 0.0);
        float pet                = MFVarGetFloat(_MDInPetID,            itemID, 0.0);
	float snowPackChg   	 = MFVarGetFloat(_MDInSnowPackChgID,    itemID, 0.0);
	float soilMoistChg  	 = MFVarGetFloat(_MDInSoilMoistChgID,   itemID, 0.0);
	float grdWaterChg   	 = MFVarGetFloat(_MDInGrdWatChgID,      itemID, 0.0);
	float runoff        	 = MFVarGetFloat(_MDInRunoffID,         itemID, 0.0);

	float runoffPoolChg 	 = MFVarGetFloat(_MDInRunoffPoolChgID,  itemID, 0.0);
	float runoffPool    	 = MFVarGetFloat(_MDInRunoffPoolID,  itemID, 0.0);
	float runoffPoolRecharge = MFVarGetFloat(_MDInRunoffPoolRechargeID,  itemID, 0.0);
	float runoffPoolRelease  = MFVarGetFloat(_MDInRunoffPoolReleaseID,  itemID, 0.0);

	float awCap        	 	 = MFVarGetFloat(_MDInSoilAvailWaterCapID,        itemID, 0.0);
	float h2oAreaFrac		 = MFVarGetFloat(_MDInH2OFractionID,              itemID, 0.0);
	float impAreaFrac		 = MFVarGetFloat(_MDInImpFractionID,              itemID, 0.0);
	float snowfall			 = MFVarGetFloat(_MDInSnowFallID,              	  itemID, 0.0);
	float snowmelt			 = MFVarGetFloat(_MDInSnowMeltID,                 itemID, 0.0);
	float snowpack			 = MFVarGetFloat(_MDInSnowPackID,                 itemID, 0.0);

	float stormRunoffImp			 = MFVarGetFloat(_MDInStormRunoffImpID,           itemID, 0.0);
	float stormRunoffH2O			 = MFVarGetFloat(_MDInStormRunoffH2OID,           itemID, 0.0);
	float stormRunoffTotal			 = MFVarGetFloat(_MDInStormRunoffTotalID,         itemID, 0.0);
	float precipPerv				 = MFVarGetFloat(_MDInPrecipPervID,               itemID, 0.0);
	float runoffToPerv				 = MFVarGetFloat(_MDInRunofftoPervID,             itemID, 0.0);

	float soilMoist					 = MFVarGetFloat(_MDInSoilMoistID,          itemID, 0.0);
	float soilMoistNotScaled		 = MFVarGetFloat(_MDInSoilMoistNotScaledID, itemID, 0.0);
	float etpNotScaled				 = MFVarGetFloat(_MDInEvaptrsNotScaledID,   itemID, 0.0);
	float excess					 = MFVarGetFloat(_MDInExcessID,             itemID, 0.0);
	float excessNotScaled			 = MFVarGetFloat(_MDInExcessNotScaledID,    itemID, 0.0);
	float soilMoistChgNotScaled		 = MFVarGetFloat(_MDInSoilMoistChgNotScaledID, itemID, 0.0);

	float surplus 					 = MFVarGetFloat(_MDInRainWaterSurplusID,   itemID, 0.0);
	float surfRunoff				 = MFVarGetFloat(_MDInRainSurfRunoffID,     itemID, 0.0);
	float infiltration				 = MFVarGetFloat(_MDInRainInfiltrationID,   itemID, 0.0);

	float grdWater					 = MFVarGetFloat(_MDInGrdWatID,             itemID, 0.0);
	float grdWaterRecharge			 = MFVarGetFloat(_MDInGrdWatRechargeID,     itemID, 0.0);
	float baseFlow					 = MFVarGetFloat(_MDInBaseFlowID,           itemID, 0.0);

	float totalSurfRunoff			 = MFVarGetFloat(_MDInTotalSurfRunoffID,    itemID, 0.0);

        float runoffVol          = MFVarGetFloat (_MDInRunoffVolumeID,  itemID, 0.0);
 	float QPre               = MFVarGetFloat (_MDInQPreID,          itemID, 0.0);
	float QOut               = MFVarGetFloat (_MDInQOutID,          itemID, 0.0);
	float QCur               = MFVarGetFloat (_MDInQCurID,          itemID, 0.0);
	float storage            = MFVarGetFloat (_MDInRiverStorageID,  itemID, 0.0);
        float C0                 = MFVarGetFloat (_MDInMuskingumC0ID,   itemID, 1.0);
	float C1                 = MFVarGetFloat (_MDInMuskingumC1ID,   itemID, 0.0);
	float C2                 = MFVarGetFloat (_MDInMuskingumC2ID,   itemID, 0.0);
        float discharge          = MFVarGetFloat (_MDInDischargeID,     itemID, 0.0);
        float storageChg         = MFVarGetFloat (_MDInRiverStorChgID,  itemID, 0.0);
        float QOut_initial       = MFVarGetFloat (_MDInDischRJSID,      itemID, 0.0);

        float propStW_RO      = MFVarGetFloat (_MDInPropROStormWaterID,   itemID, 0.0);
        float propSuW_RO      = MFVarGetFloat (_MDInPropROSurfaceWaterID, itemID, 0.0);
        float propGrW_RO      = MFVarGetFloat (_MDInPropROGroundWaterID,  itemID, 0.0);     
        float propStW_Qin     = MFVarGetFloat (_MDInPropQinStormWaterID,      itemID, 0.0);
        float propSuW_Qin     = MFVarGetFloat (_MDInPropQinSurfaceWaterID,    itemID, 0.0);
        float propGrW_Qin     = MFVarGetFloat (_MDInPropQinGroundWaterID,     itemID, 0.0);     
        float propStW_RSin    = MFVarGetFloat (_MDInPropRSinStormWaterID,     itemID, 0.0);
        float propSuW_RSin    = MFVarGetFloat (_MDInPropRSinSurfaceWaterID,   itemID, 0.0);
        float propGrW_RSin    = MFVarGetFloat (_MDInPropRSinGroundWaterID,    itemID, 0.0);
        float StW_Qout     = MFVarGetFloat (_MDInQStormWaterID,      itemID, 0.0);
        float SuW_Qout     = MFVarGetFloat (_MDInQSurfaceWaterID,    itemID, 0.0);
        float GrW_Qout     = MFVarGetFloat (_MDInQGroundWaterID,     itemID, 0.0);     
        float propStW_RSout    = MFVarGetFloat (_MDInPropRSStormWaterID,     itemID, 0.0);
        float propSuW_RSout    = MFVarGetFloat (_MDInPropRSSurfaceWaterID,   itemID, 0.0);
        float propGrW_RSout    = MFVarGetFloat (_MDInPropRSGroundWaterID,    itemID, 0.0);
        
        float propFlag_RO      = MFVarGetFloat (_MDInPropROFlagID,           itemID, 0.0);
        float propFlag_Qin     = MFVarGetFloat (_MDInPropQinFlagID,          itemID, 0.0);
        float propFlag_RSin    = MFVarGetFloat (_MDInPropRSinFlagID,         itemID, 0.0);
        float Flag_Qout        = MFVarGetFloat (_MDInQFlagID,                itemID, 0.0);
        float propFlag_RSout   = MFVarGetFloat (_MDInPropRSFlagID,           itemID, 0.0);
        
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
	float balance2;
	float balance2b;

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
	balance  = precip + irrUptakeRiver + irrUptakeExcess - (etp + runoff + grdWaterChg + snowPackChg + soilMoistChg + smallResStorageChg);
	balance2b = (precip + irrUptakeRiver + irrUptakeExcess - (etp + runoff + grdWaterChg + snowPackChg + soilMoistChg + smallResStorageChg + runoffPoolChg)) / (etp + runoff + grdWaterChg + snowPackChg + soilMoistChg + smallResStorageChg + runoffPoolChg);
	balance2 = precip + irrUptakeRiver + irrUptakeExcess - (etp + runoff + grdWaterChg + snowPackChg + soilMoistChg + smallResStorageChg + runoffPoolChg);
//	printf("d = %d, m = %d, y = %d, waterbalance = %f\n", MFDateGetCurrentDay(), MFDateGetCurrentMonth(), MFDateGetCurrentYear(), balance2);

        
 /*       
 //       if (MFDateGetCurrentYear() >= 2000) {
        if ((itemID == 1063) || (itemID == 1063)) {
//	if (fabs (balance2) > 0.0001 ) {
//		printf ("TIEM %i %d %d %d WaterBalance! %f precip %f etp = %f runoff = %f grdWaterChg = %f snowPackChg = %f soilMoistChg = %f runoffPoolChg %f\n runoffPool = %f, runoffPoolRecharge = %f, runoffPoolRelease = %f\n", itemID, MFDateGetCurrentMonth(), MFDateGetCurrentDay(), MFDateGetCurrentYear(), balance2,precip,etp,runoff,grdWaterChg,snowPackChg,soilMoistChg,runoffPoolChg,runoffPool, runoffPoolRecharge, runoffPoolRelease);
		printf ("%i, %d, %d, %d, %f, %f, %f, %f, %f, %f,", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), balance2b, balance2, awCap, impAreaFrac, h2oAreaFrac, precip);
		printf ("%f, %f, %f, %f,", snowPackChg, snowfall, snowmelt, snowpack);
		printf ("%f, %f, %f, %f, %f,", stormRunoffImp, stormRunoffH2O, stormRunoffTotal, precipPerv, runoffToPerv);
		printf ("%f, %f, %f, %f, %f, %f, %f, %f, %f,", soilMoistChg, soilMoistChgNotScaled, soilMoist, soilMoistNotScaled, pet, etp, etpNotScaled, excess, excessNotScaled);
		printf ("%f, %f, %f,", surplus, surfRunoff, infiltration);
		printf ("%f, %f, %f, %f,", grdWater, grdWaterChg, grdWaterRecharge, baseFlow);
		printf ("%f, %f, %f, %f,", runoffPool, runoffPoolChg, runoffPoolRecharge, runoffPoolRelease);
		printf ("%f, %f,", totalSurfRunoff, runoff);
                printf ("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f,", C0, C1, C2, runoffVol, discharge, QPre, QCur, QOut_initial, QOut, storage, storageChg);
                printf ("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f,", propStW_RO, propSuW_RO, propGrW_RO, propStW_Qin, propSuW_Qin, propGrW_Qin, propStW_RSin, propSuW_RSin, propGrW_RSin, StW_Qout, SuW_Qout, GrW_Qout, propStW_RSout, propSuW_RSout, propGrW_RSout, propFlag_RO, propFlag_Qin, propFlag_RSin, Flag_Qout, propFlag_RSout);
        }
//        }  
*/
   
	MFVarSetFloat (_MDOutWaterBalanceID, itemID , balance);
}

static void _MDWaterBalanceInput (int itemID) {
    	float runoff        	 = MFVarGetFloat(_MDInRunoffID,         itemID, 0.0);
        float balance            = runoff;
        
         MFVarSetFloat (_MDOutWaterBalanceID, itemID , balance);
         
}

enum { MDinput, MDcalculate, MDinput2};

int MDWaterBalanceDef() {
        int  optID = MFUnset;
	const char *optStr, *optName = MDVarRunoff;
	const char *options [] = { MDInputStr, MDCalculateStr, MDInput2Str, (char *) NULL };

	MFDefEntering ("WaterBalance");
        if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
            case MDcalculate: 	       
	if ((                                  MDAccumBalanceDef     ()  == CMfailed) ||
	    ((_MDInPrecipID                  = MDPrecipitationDef    ()) == CMfailed) ||
	    ((_MDInDischargeID               = MDDischargeDef        ()) == CMfailed) || 
	    ((_MDInSnowPackChgID             = MDSPackChgDef         ()) == CMfailed) ||
	    ((_MDInSoilMoistChgID            = MDSoilMoistChgDef     ()) == CMfailed) ||
	    ((_MDInRunoffID                  = MDRunoffDef           ()) == CMfailed) ||
//	    ((_MDInEvaptrsID                 = MFVarGetID (MDVarEvapotranspiration,      	"mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInGrdWatChgID               = MFVarGetID (MDVarGroundWaterChange,       	"mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInRunoffPoolChgID           = MFVarGetID (MDVarRunoffPoolChg,           	"mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInRunoffPoolID              = MFVarGetID (MDVarRunoffPool,         		"mm", MFInput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDInRunoffPoolRechargeID      = MFVarGetID (MDVarRunoffPoolRecharge, 		"mm", MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInRunoffPoolReleaseID       = MFVarGetID (MDVarRunoffPoolRelease,  		"mm", MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInStormRunoffImpID    		 = MFVarGetID (MDVarStormRunoffImp,  			"mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInStormRunoffH2OID    		 = MFVarGetID (MDVarStormRunoffH2O,  			"mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInPrecipPervID        		 = MFVarGetID (MDVarPrecipPerv,      			"mm",   MFInput, MFFlux, MFBoundary)) == CMfailed) ||
	    ((_MDInStormRunoffTotalID  		 = MFVarGetID (MDVarStormRunoffTotal,			"mm",   MFInput, MFFlux, MFBoundary))  == CMfailed) ||
	    ((_MDInRunofftoPervID      		 = MFVarGetID (MDVarRunofftoPerv,    			"mm",   MFInput, MFFlux, MFBoundary))  == CMfailed) ||
 	    ((_MDInPetID                        = MFVarGetID (MDVarRainPotEvapotrans,                  "mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
            ((_MDInEvaptrsID         		 = MFVarGetID (MDVarRainEvapotranspiration,     "mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
 	    ((_MDInEvaptrsNotScaledID		 = MFVarGetID (MDVarRainETnotScaled,            "mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||	// RJS 082812
	    ((_MDInSoilMoistNotScaledID		 = MFVarGetID (MDVarRainSoilMoistureNotScaled,  "mm",   MFInput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDInSoilMoistChgNotScaledID	 = MFVarGetID (MDVarRainSoilMoistureChangeNotScaled,  "mm",   MFInput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDInSoilMoistID       		 = MFVarGetID (MDVarRainSoilMoisture,           "mm",   MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInSoilAvailWaterCapID 	 	 = MFVarGetID (MDVarSoilAvailWaterCap,          "mm",   MFInput,  MFFlux, MFInitial)) == CMfailed) ||
 	    ((_MDInExcessID           		 = MFVarGetID (MDVarExcess,                     "mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||	// RJS 091813
 	    ((_MDInExcessNotScaledID    	 = MFVarGetID (MDVarExcessNotScaled,            "mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||	// RJS 091813
 	    ((_MDInImpFractionID       		 = MFVarGetID (MDVarImpFracSpatial,             "-",    MFInput,  MFState, MFBoundary)) == CMfailed) ||   // RJS 082812
	    ((_MDInH2OFractionID       	 	 = MFVarGetID (MDVarH2OFracSpatial,             "-",    MFInput,  MFState, MFBoundary)) == CMfailed) ||   // RJS 082812
	    ((_MDInQPreID       	 	 = MFVarGetID (MDVarQPre,                     "m3/s",   MFInput,  MFState, MFInitial)) == CMfailed) ||   // RJS 100213
	    ((_MDInQCurID       	 	 = MFVarGetID (MDVarQCur,                     "m3/s",   MFInput,  MFState, MFInitial)) == CMfailed) ||   // RJS 100213
            ((_MDInQOutID       	 	 = MFVarGetID (MDVarQOut,                     "m3/s",   MFInput,  MFState, MFInitial)) == CMfailed) ||   // RJS 100213
            ((_MDInDischRJSID                    = MFVarGetID (MDVarDischRJS,                 "m3/s",   MFInput,  MFState, MFInitial)) == CMfailed) ||   // RJS 100213
            ((_MDInMuskingumC0ID     	 	 = MFVarGetID (MDVarMuskingumC0,            MFNoUnit,   MFInput,  MFState, MFBoundary)) == CMfailed) ||   // RJS 100213
            ((_MDInMuskingumC1ID     	 	 = MFVarGetID (MDVarMuskingumC1,            MFNoUnit,   MFInput,  MFState, MFBoundary)) == CMfailed) ||   // RJS 100213
            ((_MDInMuskingumC2ID     	 	 = MFVarGetID (MDVarMuskingumC2,            MFNoUnit,   MFInput,  MFState, MFBoundary)) == CMfailed) ||   // RJS 100213
       	    ((_MDInRiverStorChgID                = MFVarGetID (MDVarRiverStorageChg,  "m3",     MFInput, MFFlux,  MFBoundary)) == CMfailed) ||          // RJS 100213
	    ((_MDInRiverStorageID                = MFVarGetID (MDVarRiverStorage,     "m3",     MFInput, MFState, MFInitial))  == CMfailed) ||          // RJS 100213
            ((_MDInRunoffVolumeID                = MFVarGetID (MDVarRunoffVolume,      "m3/s", MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRainWaterSurplusID            = MFVarGetID (MDVarRainWaterSurplus,     		"mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||  
            ((_MDInRainSurfRunoffID            = MFVarGetID (MDVarRainSurfRunoff,     		"mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||            
            ((_MDInRainInfiltrationID            = MFVarGetID (MDVarRainInfiltration,     		"mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
	    ((_MDInGrdWatID                	 = MFVarGetID (MDVarGroundWater,         		"mm", MFInput, MFState, MFInitial))  == CMfailed) ||
            ((_MDInGrdWatRechargeID        	 = MFVarGetID (MDVarGroundWaterRecharge, 		"mm", MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInBaseFlowID              	 = MFVarGetID (MDVarBaseFlow,            		"mm", MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInSnowFallID    			 = MFVarGetID (MDVarSnowFall,       			"mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInSnowMeltID   			 = MFVarGetID (MDVarSnowMelt,       			"mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInSnowPackID   			 = MFVarGetID (MDVarSnowPack,       			"mm",   MFInput, MFState, MFInitial))  == CMfailed) ||
            ((_MDInTotalSurfRunoffID 		 = MFVarGetID (MDVarTotalSurfRunoff, 			"mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
            ((_MDInPropROStormWaterID      = MFVarGetID (MDVarPropROStormWater,    "-",   MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313
            ((_MDInPropROSurfaceWaterID    = MFVarGetID (MDVarPropROSurfaceWater,  "-",   MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313 
            ((_MDInPropROGroundWaterID     = MFVarGetID (MDVarPropROGroundWater,   "-",   MFInput, MFState, MFBoundary)) == CMfailed) ||   
            ((_MDInQStormWaterID         = MFVarGetID (MDVarQStormWater,    "m3/s",    MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313
            ((_MDInQSurfaceWaterID       = MFVarGetID (MDVarQSurfaceWater,  "m3/s",    MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313 
            ((_MDInQGroundWaterID        = MFVarGetID (MDVarQGroundWater,   "m3/s",    MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313  
	    ((_MDInPropRSStormWaterID        = MFVarGetID (MDVarPropRSStormWater,   "-",   MFInput, MFState, MFInitial)) == CMfailed) ||       // RJS 100313
            ((_MDInPropRSSurfaceWaterID      = MFVarGetID (MDVarPropRSSurfaceWater, "-",   MFInput, MFState, MFInitial)) == CMfailed) ||       // RJS 100313 
            ((_MDInPropRSGroundWaterID       = MFVarGetID (MDVarPropRSGroundWater,  "-",   MFInput, MFState, MFInitial)) == CMfailed) ||       // RJS 100313  	    
            ((_MDInPropQinStormWaterID    = MFVarGetID (MDVarPropQinStormWater,   "-",   MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313
            ((_MDInPropQinSurfaceWaterID  = MFVarGetID (MDVarPropQinSurfaceWater,   "-", MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313
            ((_MDInPropQinGroundWaterID   = MFVarGetID (MDVarPropQinGroundWater,   "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313
            ((_MDInPropRSinStormWaterID   = MFVarGetID (MDVarPropRSinStormWater,   "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313
            ((_MDInPropRSinSurfaceWaterID = MFVarGetID (MDVarPropRSinSurfaceWater,  "-", MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313
            ((_MDInPropRSinGroundWaterID  = MFVarGetID (MDVarPropRSinGroundWater,   "-", MFInput, MFState, MFBoundary)) == CMfailed) ||       // RJS 100313            
            ((_MDInPropROFlagID           = MFVarGetID (MDVarPropROFlag,          "-",   MFInput, MFState, MFBoundary)) == CMfailed) ||
            ((_MDInQFlagID                = MFVarGetID (MDVarQFlag,            "m3/s",   MFInput, MFState, MFBoundary)) == CMfailed) ||
            ((_MDInPropRSFlagID           = MFVarGetID (MDVarPropRSFlag,       "-",      MFInput, MFState, MFInitial)) == CMfailed) || 
            ((_MDInPropQinFlagID          = MFVarGetID (MDVarPropQinFlag,      "-",      MFInput, MFState, MFBoundary))  == CMfailed) ||
            ((_MDInPropRSinFlagID         = MFVarGetID (MDVarPropRSinFlag,     "-",      MFInput, MFState, MFBoundary))  == CMfailed) ||
            ((_MDOutWaterBalanceID        = MFVarGetID (MDVarWaterBalance,      "mm",    MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction(_MDWaterBalance) == CMfailed))
	    return (CMfailed);
        break;      
                case MDinput: 																										       			// RJS 061312
                if (((_MDInRunoffID               = MDRunoffDef           ()) == CMfailed) ||			
		    ((_MDOutWaterBalanceID        = MFVarGetID (MDVarWaterBalance,      "mm",    MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
		    (MFModelAddFunction (_MDWaterBalanceInput) == CMfailed)) return (CMfailed);														// RJS 061312
		break;	
                case MDinput2: 																										       			// RJS 061312
                if (((_MDInRunoffID               = MDRunoffDef           ()) == CMfailed) ||			
		    ((_MDOutWaterBalanceID        = MFVarGetID (MDVarWaterBalance,      "mm",    MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
		    (MFModelAddFunction (_MDWaterBalanceInput) == CMfailed)) return (CMfailed);														// RJS 061312
		break;	
        default: MFOptionMessage (optName, optStr, options); return (CMfailed);
        }
	MFDefLeaving ("WaterBalance");
	return (_MDOutWaterBalanceID);	
}
