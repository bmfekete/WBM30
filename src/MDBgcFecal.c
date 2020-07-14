/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDFecal.c  - Routing and processing of fecal coliform

wil.wollheim@unh.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

static int _MDInDischargeID       = MFUnset;
static int _MDInRiverWidthID      = MFUnset;
static int _MDInRiverDepthID      = MFUnset;
//static int _MDInRiverVelocityID   = MFUnset;
static int _MDInRunoffVolumeID    = MFUnset;
static int _MDInRunoffID          = MFUnset;
static int _MDInRiverStorageChgID = MFUnset;
static int _MDInRiverStorageID    = MFUnset;
static int _MDInResCapacityID     = MFUnset;
static int _MDInResStorageID      = MFUnset;

static int _MDPointSources_UrbanPopID  = MFUnset;

static float _MDFecalK				   = MFUnset;

static int _MDConc_FecalID             = MFUnset;
static int _MDFlux_FecalID             = MFUnset;
static int _MDLocalIn_FecalID          = MFUnset;
static int _MDRemoval_FecalID          = MFUnset;
static int _MDStorage_FecalID          = MFUnset;
static int _MDDeltaStorage_FecalID     = MFUnset;

static int _MDConcMixing_FecalID       = MFUnset;
static int _MDFluxMixing_FecalID       = MFUnset;
static int _MDStorageMixing_FecalID    = MFUnset;
static int _MDDeltaStorageMixing_FecalID= MFUnset;

static void _MDFecalRouting (int itemID) {
//input	
		float discharge;
		float width;
		float depth;
		float velocity;
        float waterStorageChange;
		float waterStorage;
		
		float urbanPop;
		float fecalLocalIn;
		float fecalFlux;
		float fecalFluxNew;
        float fecalStorage;
		float fecalStorageNew;
        float fecalDeltaStorage;
        float fecalTotalIn;
        float fecalConcentration; // # / 100 ml
        
        float fecalFluxMixing;
        float fecalFluxNewMixing;
        float fecalStorageMixing;
        float fecalStorageNewMixing;
        float fecalDeltaStorageMixing;
        float fecalTotalInMixing;
        float fecalConcentrationMixing;
        
        float fecalConst_NumPerCap = 2000000000; //# per day from Gibson 2006
        //float fecalConst_NumPerCap = 8.7 * pow(10,10); //# 

        float STP_removal = 0.95;
	    //float fecalK = 0.7;	//d-1
	    float fecalK;	//d-1
        float fecalRemoval;
		float massbalance;
		float massbalanceMixing;

		discharge                  = MFVarGetFloat (_MDInDischargeID, itemID, 0.0);
		width                      = MFVarGetFloat (_MDInRiverWidthID, itemID, 0.0);
	    //velocity                 = MFVarGetFloat (_MDInRiverVelocityID, itemID, 0.0);
	    depth                      = MFVarGetFloat (_MDInRiverDepthID, itemID, 0.0);
        urbanPop                   = MFVarGetFloat (_MDPointSources_UrbanPopID, itemID, 0.0);
	   	fecalFlux                  = MFVarGetFloat (_MDFlux_FecalID, itemID, 0.0);
        fecalStorage			   = MFVarGetFloat (_MDStorage_FecalID, itemID, 0.0);
        fecalFluxMixing            = MFVarGetFloat (_MDFluxMixing_FecalID, itemID, 0.0);
        fecalStorageMixing		   = MFVarGetFloat (_MDStorageMixing_FecalID, itemID, 0.0);
        fecalK					   = _MDFecalK;
        
        waterStorageChange = MFVarGetFloat ( _MDInRiverStorageChgID,    itemID, 0.0);
        waterStorage       = MFVarGetFloat ( _MDInRiverStorageID,       itemID, 0.0);
		
        velocity = discharge / (width * depth);  // m/s
        fecalLocalIn = urbanPop * fecalConst_NumPerCap * (1 - STP_removal); //#/d
		fecalTotalIn = fecalFlux + fecalLocalIn + fecalStorage;
		fecalTotalInMixing = fecalFluxMixing + fecalLocalIn + fecalStorageMixing;
		
		//if(!isnan(discharge) && (discharge > 0.0)) {
	    if(!isnan(discharge) && (discharge > 0.001)) {
		    fecalRemoval = fecalTotalIn * (1 - exp(-fecalK * (MFModelGetLength(itemID) / (velocity * 86400))));
			fecalConcentration = ((fecalTotalIn - fecalRemoval) / (discharge * MDConst_m3PerSecTOm3PerDay + waterStorage)) / (1000 * 10); // #/100ml
		    fecalStorageNew = waterStorage * (fecalConcentration * 10 * 1000);
		    fecalDeltaStorage = fecalStorageNew - fecalStorage;
		    fecalFluxNew = discharge * 86400 * (fecalConcentration * 1000 * 10); //#/d
		    
		    fecalConcentrationMixing = ((fecalTotalInMixing) / (discharge * MDConst_m3PerSecTOm3PerDay + waterStorage)) / (1000 * 10); // #/100ml
		    fecalStorageNewMixing = waterStorage * (fecalConcentrationMixing * 10 * 1000);
		    fecalFluxNewMixing = discharge * 86400 * (fecalConcentrationMixing * 1000 * 10); //#/d
		    fecalDeltaStorageMixing = fecalStorageNewMixing - fecalStorageMixing;
            
	 	    MFVarSetFloat(_MDLocalIn_FecalID,itemID,fecalLocalIn); 
	 	    MFVarSetFloat(_MDFlux_FecalID,itemID,fecalFluxNew); 
	 	    MFVarSetFloat(_MDStorage_FecalID,itemID,fecalStorageNew); 
	 	    MFVarSetFloat(_MDDeltaStorage_FecalID,itemID,fecalDeltaStorage); 
            MFVarSetFloat(_MDRemoval_FecalID,itemID,fecalRemoval); 
            MFVarSetFloat(_MDConc_FecalID,itemID,fecalConcentration);
            
            MFVarSetFloat(_MDFluxMixing_FecalID,itemID,fecalFluxNewMixing); 
            MFVarSetFloat(_MDStorageMixing_FecalID,itemID,fecalStorageNewMixing); 
            MFVarSetFloat(_MDConcMixing_FecalID,itemID,fecalConcentrationMixing);
	 	    MFVarSetFloat(_MDDeltaStorageMixing_FecalID,itemID,fecalDeltaStorageMixing); 

	    }
		else {
			fecalRemoval = fecalTotalIn;
			fecalDeltaStorage = -fecalStorage; 
			fecalDeltaStorageMixing = -fecalStorageMixing;

			MFVarSetFloat(_MDLocalIn_FecalID,itemID,fecalLocalIn); 
	 	    MFVarSetFloat(_MDFlux_FecalID,itemID,0.0); 
	 	    MFVarSetFloat(_MDStorage_FecalID,itemID,0.0); 
	 	    MFVarSetFloat(_MDDeltaStorage_FecalID,itemID,fecalDeltaStorage); 
            MFVarSetFloat(_MDRemoval_FecalID,itemID,fecalRemoval); 
            MFVarSetMissingVal(_MDConc_FecalID,itemID);
            MFVarSetFloat(_MDFluxMixing_FecalID,itemID,0.0); 
            MFVarSetFloat(_MDStorageMixing_FecalID,itemID,0.0); 
            MFVarSetMissingVal(_MDConcMixing_FecalID,itemID);
	 	    MFVarSetFloat(_MDDeltaStorageMixing_FecalID,itemID,fecalDeltaStorageMixing); 

		}
    //massbalance = fecalTotalIn - fecalRemoval - fecalStorageNew - fecalFluxNew;
	    if (itemID == 32) printf (" FecalK %f \n", fecalK);
	massbalance = (fecalFlux + fecalLocalIn - (fecalRemoval + fecalDeltaStorage + fecalFluxNew)) / fecalTotalIn * 100;
    massbalanceMixing = (fecalFluxMixing + fecalLocalIn - (fecalDeltaStorageMixing + fecalFluxNewMixing)) / fecalTotalInMixing * 100;
	if(!isnan(discharge) && (discharge > 0.001)) {
       if ((massbalance > 0.001) || (massbalanceMixing > 0.001)) {
    	   //printf ("discharge %f fecalFlux %f fecalLocalIn %f fecalStorage %f fecalRemoval %f fecalStorageNew %f fecalFluxNew %f MassBalance %f \n", 
       //       							discharge, fecalFlux / 1000000, fecalLocalIn / 1000000 , fecalStorage / 1000000, fecalRemoval / 1000000, fecalStorageNew / 1000000, fecalFluxNew / 1000000, massbalance);
          //if (itemID == 32) printf ("discharge %f fecalFlux %f fecalLocalIn %f fecalStorage %f fecalRemoval %f fecalStorageNew %f fecalFluxNew %f MassBalance %f \n", 
          //							discharge, fecalFlux, fecalLocalIn, fecalStorage, fecalRemoval, fecalStorageNew, fecalFluxNew, massbalance);
           printf (" MassBalance %f \n", massbalance);
           printf (" MassBalanceMixing %f \n", massbalanceMixing);
       }
	}    
	//if (itemID == 338) printf ("Year %d Month %d length %f discharge %f velocity %f \n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFModelGetLength(itemID), discharge, velocity);
		//if (itemID == 338) printf ("Year %d Month %d discharge %f urbanPop %f fecalLocalIn %f fecalTotalIn %f fecalConcentration %f \n", 
		//		MFDateGetCurrentYear(), MFDateGetCurrentMonth(), discharge, urbanPop, fecalLocalIn, fecalTotalIn, fecalConcentration);   

}

int MDFecalRoutingDef () {
	float par;
	char *optStr;
	char *options [] = { MDNoneStr, (char *) NULL };

	if (_MDFlux_FecalID != MFUnset)	return (_MDFlux_FecalID);

	MFDefEntering ("Fecal Coliform Calculation");
	if (((optStr = MFOptionGet (MDParFecalK))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDFecalK = par;
		
   // Input
	if (((_MDInDischargeID           = MDDischargeDef    ()) == CMfailed) ||
        ((_MDInRiverWidthID          = MDRiverWidthDef   ()) == CMfailed) ||
	    ((_MDInRunoffVolumeID        = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDInRunoffID              = MDRunoffDef ()) == CMfailed) ||
	    ((_MDInRiverDepthID          = MFVarGetID (MDVarRiverDepth,             "m",       MFInput, MFState, MFBoundary))  == CMfailed) ||
	    //((_MDInRiverVelocityID     = MFVarGetID (MDVarRiverMeanVelocity,      "m/s",     MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDInRiverStorageChgID     = MFVarGetID (MDVarRiverStorageChg,        "m3/s",    MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDInRiverStorageID        = MFVarGetID (MDVarRiverStorage,           "m3",      MFInput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDPointSources_UrbanPopID          = MFVarGetID (MDVarPoint_Fecal,             "#",   MFInput, MFState, MFBoundary))  == CMfailed) ||
	    
	    // Output
	    ((_MDLocalIn_FecalID                = MFVarGetID (MDVarFecalLocalIn,                "#/d",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
	    ((_MDRemoval_FecalID                = MFVarGetID (MDVarFecalRemoval,                "#/d",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
        ((_MDConc_FecalID                   = MFVarGetID (MDVarFecalConcentration,          "#/100ml",   MFOutput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDStorage_FecalID                = MFVarGetID (MDVarFecalStorage,                "#",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDDeltaStorage_FecalID           = MFVarGetID (MDVarFecalDeltaStorage,           "#/day",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
	    //((_MDLoad_FecalID                 = MFVarGetID (MDVarFecalLoad,                    "#/day",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
         ((_MDFlux_FecalID                  = MFVarGetID (MDVarFecalFlux  ,                  "#/day",  MFRoute,  MFFlux, MFBoundary))  == CMfailed) ||
         ((_MDConcMixing_FecalID            = MFVarGetID (MDVarFecalConcentration_Mixing,    "#/100ml",   MFOutput, MFState, MFBoundary))  == CMfailed) ||
         ((_MDStorageMixing_FecalID         = MFVarGetID (MDVarFecalStorage_Mixing,          "#",   MFOutput, MFState, MFInitial))  == CMfailed) ||
         ((_MDDeltaStorageMixing_FecalID    = MFVarGetID (MDVarFecalDeltaStorage_Mixing,     "#/day",   MFOutput, MFState, MFBoundary))  == CMfailed) ||
         ((_MDFluxMixing_FecalID            = MFVarGetID (MDVarFecalFlux_Mixing ,            "#/day",  MFRoute,  MFFlux, MFBoundary))  == CMfailed) |
        (MFModelAddFunction (_MDFecalRouting) == CMfailed)) return (CMfailed); 

	if (((optStr = MFOptionGet (MDOptReservoirs)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
		if (((_MDInResCapacityID       = MFVarGetID (MDVarReservoirCapacity,      "km3",     MFInput, MFState, MFBoundary))  == CMfailed) ||
		    ((_MDInResStorageID        = MFVarGetID (MDVarReservoirStorage,       "km3",     MFInput, MFState, MFInitial))   == CMfailed))
	    	return (CMfailed);
	}
	MFDefLeaving ("Fecal Coliform Calculation");
	return (_MDConc_FecalID); 
}
